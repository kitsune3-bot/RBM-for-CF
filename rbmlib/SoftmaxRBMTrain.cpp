#include "SoftmaxRBMTrain.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxDataset.h"
#include "SoftmaxRBM.h"
#include "SoftmaxRBMBuilder.h"




SoftmaxRBMTrain::SoftmaxRBMTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate)
{
	this->vlength = vlen;
	this->klength = klen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->_tmpGradientVbias = std::vector< std::vector<double> >(vlen, std::vector<double>(klen, 0));
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector< std::vector<double> > >(vlen, std::vector < std::vector<double> >(klen, std::vector<double>(hlen, 0)));

}

SoftmaxRBMTrain::SoftmaxRBMTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate, unsigned int cdk)
{
	this->vlength = vlen;
	this->klength = klen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->cdk = cdk;
	this->_tmpGradientVbias = std::vector< std::vector<double> >(vlen, std::vector<double>(klen, 0));
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector< std::vector<double> > >(vlen, std::vector < std::vector<double> >(klen, std::vector<double>(hlen, 0)));
}


SoftmaxRBMTrain::~SoftmaxRBMTrain()
{
}

bool SoftmaxRBMTrain::train(SoftmaxRBM & rbm, SoftmaxDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			this->_getGradient(rbm, datasets[dat_counter]);

			//勾配逐次更新
			//vbias更新
			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
				{
					double gradient = this->learningRate * this->_tmpGradientVbias[v_counter][k_counter];
					double new_vbias = rbm.vbias[v_counter][k_counter] + gradient;
					rbm.vbias[v_counter][k_counter] = new_vbias;
					//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
				}
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				double gradient = this->learningRate * this->_tmpGradientHbias[h_counter];
				double new_hbias = rbm.hbias[h_counter] + gradient;
				rbm.hbias[h_counter] = new_hbias;
				//printf("hbias[%d]: %e\n", hnode_count, gradient);
			}

			//weight更新
			for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
				for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
				{
					for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
						double gradient = this->learningRate * this->_tmpGradientWeight[v_count][k_count][h_count];
						double new_weight = rbm.weight[v_count][k_count][h_count] + gradient;
						rbm.weight[v_count][k_count][h_count] = new_weight;
						//printf("weight[%d][%d][%d]: %e\n", v_count, k_count, h_count, gradient);
					}
				}
			}
		}
	}
	//カルバックライブラー情報量は?

	return true;
}

bool SoftmaxRBMTrain::trainBatch(SoftmaxRBM & rbm, SoftmaxDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		auto gradient_vbias = std::vector< std::vector<double> >(rbm.vlength, std::vector<double>(rbm.k, 0));
		auto gradient_hbias = std::vector<double>(rbm.hlength, 0);
		auto gradient_weight = std::vector< std::vector< std::vector<double> > >(rbm.vlength, std::vector < std::vector<double> >(rbm.k, std::vector<double>(rbm.hlength, 0)));


		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			this->_getGradient(rbm, datasets[dat_counter]);

			//勾配
			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
				{
					double gradient = this->_tmpGradientVbias[v_counter][k_counter];
					gradient_vbias[v_counter][k_counter] += gradient;
				}
			}

			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				double gradient = this->_tmpGradientHbias[h_counter];
				gradient_hbias[h_counter] += gradient;
			}

			for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
				for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
				{
					for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
						double gradient = this->_tmpGradientWeight[v_count][k_count][h_count];
						gradient_weight[v_count][k_count][h_count] += gradient;
					}
				}
			}
		}

		//勾配更新
		//vbias更新
		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
			for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
			{
				double gradient = this->learningRate * (gradient_vbias[v_counter][k_counter] / (double)datasets.getLength());
				double new_vbias = rbm.vbias[v_counter][k_counter] + gradient;
				rbm.vbias[v_counter][k_counter] = new_vbias;
				//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
			}
		}

		//hbias更新
		for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
			double gradient = this->learningRate * (gradient_hbias[h_counter] / (double)datasets.getLength());
			double new_hbias = rbm.hbias[h_counter] + gradient;
			rbm.hbias[h_counter] = new_hbias;
			//printf("hbias[%d]: %e\n", hnode_count, gradient);
		}

		//weight更新
		for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
			for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
			{
				for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
					double gradient = this->learningRate * (gradient_weight[v_count][k_count][h_count] / (double)datasets.getLength());
					double new_weight = rbm.weight[v_count][k_count][h_count] + gradient;
					rbm.weight[v_count][k_count][h_count] = new_weight;
					//printf("weight[%d][%d][%d]: %e\n", v_count, k_count, h_count, gradient);
				}
			}
		}
	}
	//カルバックライブラー情報量は?

	return true;
}


bool SoftmaxRBMTrain::_getGradient(SoftmaxRBM & rbm, SoftmaxDataset & dataset)
{
	//有効可視変数のみのRBM
	SoftmaxRBMBuilder::setDataset(rbm, dataset);

	//data項
	std::vector<std::vector<SoftmaxRBM::VNODETYPE> > v0 = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(rbm.vlength, std::vector<SoftmaxRBM::VNODETYPE>(rbm.k));
	std::vector<double> h0 = std::vector<double>(rbm.hlength);

	//v0セット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
		{
			v0[v_counter][k_counter] = rbm.vnodes[v_counter][k_counter];
		}
	}

	//h0セット
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++)
	{
		double tmp_sum_w = 0;//TODO: sum_weight()

		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
		{
			for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
			{
				tmp_sum_w += rbm.weight[v_counter][k_counter][h_counter] * rbm.vnodes[v_counter][k_counter];
			}
		}

		h0[h_counter] = 1.0 / (1.0 + exp(-(tmp_sum_w + rbm.hbias[h_counter])));
	}

	//model項
	std::vector<std::vector<SoftmaxRBM::VNODETYPE> > vt = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(rbm.vlength, std::vector<SoftmaxRBM::VNODETYPE>(rbm.k));
	std::vector<double> ht = std::vector<double>(rbm.hlength);
	this->_contrastiveDivergence(&rbm, this->cdk);

	//vtセット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
		{
			vt[v_counter][k_counter] = rbm.vnodes[v_counter][k_counter];
		}
	}

	//htセット
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++)
	{
		ht[h_counter] = rbm.hnodes[h_counter];
	}


	//勾配セット
	//vbiasセット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
		for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
		{
			double gradient = (v0[v_counter][k_counter] - vt[v_counter][k_counter]);
			this->_tmpGradientVbias[v_counter][k_counter] = gradient;
		}
	}

	//hbiasセット
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
		double gradient = (h0[h_counter] - ht[h_counter]);
		this->_tmpGradientHbias[h_counter] = gradient;
	}

	//weightセット
	for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
		for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
		{
			for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
				double gradient = (v0[v_count][k_count] * h0[h_count] - vt[v_count][k_count] * ht[h_count]);
				this->_tmpGradientWeight[v_count][k_count][h_count] = gradient;
			}
		}
	}

	return true;
}

bool SoftmaxRBMTrain::_contrastiveDivergence(SoftmaxRBM *rbm, unsigned int cdk)
{
	rbm->gibbsSampleHidden();
	rbm->gibbsSample(cdk);

	return true;
}
