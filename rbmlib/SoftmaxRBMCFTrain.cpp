#include "SoftmaxRBMCFTrain.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxDataset.h"
#include "SoftmaxRBM.h"
#include "SoftmaxRBMCF.h"
#include "SoftmaxRBMCFBuilder.h"



SoftmaxRBMCFTrain::SoftmaxRBMCFTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate)
{
	this->vlength = vlen;
	this->klength = klen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->_tmpGradientVbias = std::vector< std::vector<double> >(vlen, std::vector<double>(klen, 0));
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector< std::vector<double> > >(vlen, std::vector < std::vector<double> >(klen, std::vector<double>(hlen, 0)));
	this->_table = std::vector<unsigned int>(vlen, 0);
}

SoftmaxRBMCFTrain::SoftmaxRBMCFTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate, unsigned int cdk)
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
	this->_table = std::vector<unsigned int>(vlen, 0);
}


SoftmaxRBMCFTrain::~SoftmaxRBMCFTrain()
{
}


bool SoftmaxRBMCFTrain::train(SoftmaxRBMCF & rbm, SoftmaxDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		//逐次更新
		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			//有効可視変数のみのRBM
			SoftmaxRBMCFBuilder::setDataset(rbm, datasets[dat_counter]);
			SoftmaxRBM *available_rbm = rbm.getAvailableRBM();
			SoftmaxRBM *tmp_arbm = SoftmaxRBMBuilder::duplicate(available_rbm);

			//data項目
			std::vector<std::vector<SoftmaxRBM::VNODETYPE> > v0 = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(tmp_arbm->vlength, std::vector<SoftmaxRBM::VNODETYPE>(tmp_arbm->k));
			std::vector<double> h0 = std::vector<double>(tmp_arbm->hlength);

			//v0セット
			for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
			{
				for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
				{
					v0[v_counter][k_counter] = tmp_arbm->vnodes[v_counter][k_counter];
				}
			}

			//h0セット
			for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
			{
				double tmp_sum_wv = 0;

				for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
				{
					for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
					{
						tmp_sum_wv += tmp_arbm->weight[v_counter][k_counter][h_counter]* tmp_arbm->vnodes[v_counter][k_counter];
					}
				}

				h0[h_counter] = 1.0 / (1.0 + exp(-(tmp_sum_wv + tmp_arbm->hbias[h_counter])));
			}

			//model項
			std::vector<std::vector<SoftmaxRBM::VNODETYPE> > vt = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(tmp_arbm->vlength, std::vector<SoftmaxRBM::VNODETYPE>(tmp_arbm->k));
			std::vector<double> ht = std::vector<double>(tmp_arbm->hlength);
			this->_contrastiveDivergence(tmp_arbm, this->cdk);

			//vtセット
			for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
			{
				for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
				{
					vt[v_counter][k_counter] = tmp_arbm->vnodes[v_counter][k_counter];
				}
			}

			//htセット
			for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
			{
				ht[h_counter] = tmp_arbm->hnodes[h_counter];
			}


			//勾配更新
			//vbias更新
			for (unsigned int v_counter = 0; v_counter < available_rbm->vlength; v_counter++) {
				for (unsigned int k_counter = 0; k_counter < available_rbm->k; k_counter++)
				{
					double gradient = this->learningRate * (v0[v_counter][k_counter] - vt[v_counter][k_counter]);
					double new_vbias = available_rbm->vbias[v_counter][k_counter] + gradient;
					available_rbm->vbias[v_counter][k_counter] = new_vbias;
					//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
				}
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < available_rbm->hlength; h_counter++) {
				double gradient = this->learningRate * (h0[h_counter] - ht[h_counter]);
				double new_hbias = available_rbm->hbias[h_counter] + gradient;
				available_rbm->hbias[h_counter] = new_hbias;
				//printf("hbias[%d]: %e\n", hnode_count, gradient);
			}

			//weight更新
			for (unsigned int v_count = 0; v_count < available_rbm->vlength; v_count++)
			{
				for (unsigned int k_count = 0; k_count < available_rbm->k; k_count++)
				{
					for (unsigned int h_count = 0; h_count < available_rbm->hlength; h_count++) {
						double gradient = this->learningRate * (v0[v_count][k_count] * h0[h_count] - vt[v_count][k_count] * ht[h_count]);
						double new_weight = available_rbm->weight[v_count][k_count][h_count] + gradient;
						available_rbm->weight[v_count][k_count][h_count] = new_weight;
						//printf("weight[%d][%d][%d]: %e\n", v_count, k_count, h_count, gradient);
					}
				}
			}

			//元のRBMにパラメータを設定
			rbm.setAvailableRBM(available_rbm);

			//後始末
			delete available_rbm;
			delete tmp_arbm;
		}
	}
	//カルバックライブラー情報量は?

	return true;
}

bool SoftmaxRBMCFTrain::trainBatch(SoftmaxRBMCF & rbm, SoftmaxDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		auto gradient_vbias = std::vector< std::vector<double> >(rbm.vlength, std::vector<double>(rbm.k, 0));
		auto gradient_hbias = std::vector<double>(rbm.hlength, 0);
		auto gradient_weight = std::vector< std::vector< std::vector<double> > >(rbm.vlength, std::vector < std::vector<double> >(rbm.k, std::vector<double>(rbm.hlength, 0)));

		this->_tmpGradientVbias = std::vector< std::vector<double> >(this->vlength, std::vector<double>(this->klength, 0));
		this->_tmpGradientHbias = std::vector<double>(this->hlength, 0);
		this->_tmpGradientWeight = std::vector< std::vector< std::vector<double> > >(this->vlength, std::vector < std::vector<double> >(this->klength, std::vector<double>(this->hlength, 0)));
		this->_table.assign(this->vlength, 0);

		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			this->_getGradient(rbm, datasets[dat_counter]);

			//平均用テーブル更新
			{
				auto enable_table = rbm.getAvailableTable();
				for (auto & vno : enable_table)
				{
					this->_table[vno]++;
				}
			}


			//勾配更新
			//vbias更新
			auto tmp_table = rbm.getAvailableTable();
			for (auto & vno : tmp_table)
			{
				for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
				{
					gradient_vbias[vno][k_counter] += this->_tmpGradientVbias[vno][k_counter];
				}
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				gradient_hbias[h_counter] += this->_tmpGradientHbias[h_counter];
			}

			//weight更新
			for (auto & vno : tmp_table)
			{
				for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
				{
					for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
						gradient_weight[vno][k_count][h_count] += this->_tmpGradientWeight[vno][k_count][h_count];
					}
				}
			}


		}



		//勾配更新
		//vbias更新
		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
			for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
			{
				double gradient = this->learningRate * (gradient_vbias[v_counter][k_counter] / (double)this->_table[v_counter]);
				double new_vbias = rbm.vbias[v_counter][k_counter] + gradient;
				rbm.vbias[v_counter][k_counter] = new_vbias;
			}
		}

		//hbias更新
		for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
			double gradient = this->learningRate * (gradient_hbias[h_counter] / (double)datasets.getLength());
			double new_hbias = rbm.hbias[h_counter] + gradient;
			rbm.hbias[h_counter] = new_hbias;
		}

		//weight更新
		for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
			for (unsigned int k_count = 0; k_count < rbm.k; k_count++)
			{
				for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
					double gradient = this->learningRate * (gradient_weight[v_count][k_count][h_count] / (double)this->_table[v_count]);
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

bool SoftmaxRBMCFTrain::_getGradient(SoftmaxRBMCF & rbm, SoftmaxDataset & dataset)
{
	//有効可視変数のみのRBM
	SoftmaxRBMCFBuilder::setDataset(rbm, dataset);
	SoftmaxRBM *available_rbm = rbm.getAvailableRBM();
	SoftmaxRBM *tmp_arbm = SoftmaxRBMBuilder::duplicate(available_rbm);

	//data項目
	std::vector<std::vector<SoftmaxRBM::VNODETYPE> > v0 = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(tmp_arbm->vlength, std::vector<SoftmaxRBM::VNODETYPE>(tmp_arbm->k));
	std::vector<double> h0 = std::vector<double>(tmp_arbm->hlength);

	//v0セット
	for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
		{
			v0[v_counter][k_counter] = tmp_arbm->vnodes[v_counter][k_counter];
		}
	}

	//h0セット
	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		double tmp_sum_wv = 0;

		for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
		{
			for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
			{
				tmp_sum_wv += tmp_arbm->weight[v_counter][k_counter][h_counter] * tmp_arbm->vnodes[v_counter][k_counter];
			}
		}

		h0[h_counter] = 1.0 / (1.0 + exp(-(tmp_sum_wv + tmp_arbm->hbias[h_counter])));
	}

	//model項
	std::vector<std::vector<SoftmaxRBM::VNODETYPE> > vt = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(tmp_arbm->vlength, std::vector<SoftmaxRBM::VNODETYPE>(tmp_arbm->k));
	std::vector<double> ht = std::vector<double>(tmp_arbm->hlength);
	this->_contrastiveDivergence(tmp_arbm, this->cdk);

	//vtセット
	for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
		{
			vt[v_counter][k_counter] = tmp_arbm->vnodes[v_counter][k_counter];
		}
	}

	//htセット
	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		ht[h_counter] = tmp_arbm->hnodes[h_counter];
	}

	//勾配セット
	//vbias更新
	for (unsigned int v_counter = 0; v_counter < available_rbm->vlength; v_counter++) {

		unsigned int vno = rbm.getAvailableTable()[v_counter];

		for (unsigned int k_counter = 0; k_counter < available_rbm->k; k_counter++)
		{
			double gradient = (v0[v_counter][k_counter] - vt[v_counter][k_counter]);
			this->_tmpGradientVbias[vno][k_counter] = gradient;
		}
	}

	//hbias更新
	for (unsigned int h_counter = 0; h_counter < available_rbm->hlength; h_counter++) {
		double gradient = (h0[h_counter] - ht[h_counter]);
		this->_tmpGradientHbias[h_counter] = gradient;
	}

	//weight更新
	for (unsigned int v_count = 0; v_count < available_rbm->vlength; v_count++)
	{
		unsigned int vno = rbm.getAvailableTable()[v_count];

		for (unsigned int k_count = 0; k_count < available_rbm->k; k_count++)
		{
			for (unsigned int h_count = 0; h_count < available_rbm->hlength; h_count++) {
				double gradient = (v0[v_count][k_count] * h0[h_count] - vt[v_count][k_count] * ht[h_count]);
				this->_tmpGradientWeight[vno][k_count][h_count] = gradient;
			}
		}

	}



	//元のRBMにパラメータを設定
	//rbm.setAvailableRBM(available_rbm);

	//後始末
	delete available_rbm;
	delete tmp_arbm;


	return true;
}

bool SoftmaxRBMCFTrain::_contrastiveDivergence(SoftmaxRBM *rbm, unsigned int cdk)
{
	rbm->gibbsSampleHidden();
	rbm->gibbsSample(cdk);

	return true;
}

