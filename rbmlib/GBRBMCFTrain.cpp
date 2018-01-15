#include "GBRBMCFTrain.h"
#include "GBRBMDatasetContainer.h"
#include "GBRBMCFBuilder.h"
#include "GBRBMCF.h"



GBRBMCFTrain::GBRBMCFTrain(unsigned int vlen, unsigned int hlen, unsigned int epoch, double leaning_rate)
{
	this->vlength = vlen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->_tmpGradientVbias = std::vector<double>(vlen, 0);
	this->_tmpGradientVariance = std::vector<double>(vlen, 0);
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector<double> >(vlen, std::vector<double>(hlen, 0));
	this->_table = std::vector<unsigned int>(vlen, 0);
}

GBRBMCFTrain::GBRBMCFTrain(unsigned int vlen, unsigned int hlen, unsigned int epoch, double leaning_rate, unsigned int cdk)
{
	this->vlength = vlen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->cdk = cdk;
	this->_tmpGradientVbias = std::vector<double>(vlen, 0);
	this->_tmpGradientVariance = std::vector<double>(vlen, 0);
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector<double> >(vlen, std::vector<double>(hlen, 0));
	this->_table = std::vector<unsigned int>(vlen, 0);
}


GBRBMCFTrain::~GBRBMCFTrain()
{
}

bool GBRBMCFTrain::train(GBRBMCF & rbm, GBRBMDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		//逐次更新
		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			//有効可視変数のみのRBM
			GBRBMCFBuilder::setDataset(rbm, datasets[dat_counter]);
			GBRBM *available_rbm = rbm.getAvailableRBM();
			GBRBM *tmp_arbm = GBRBMBuilder::duplicate(available_rbm);


			//data項
			std::vector<GBRBM::VNODETYPE> v0 = std::vector<GBRBM::VNODETYPE>(tmp_arbm->vlength, 0);
			std::vector<double> h0 = std::vector<double>(tmp_arbm->hlength);
			//v0セット
			for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
			{
				v0[v_counter] = tmp_arbm->vnodes[v_counter];
			}

			//h0セット
			for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
			{
				double tmp_sum_w = 0;

				for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
				{
					tmp_sum_w += tmp_arbm->weight[v_counter][h_counter] * tmp_arbm->vnodes[v_counter];
				}

				h0[h_counter] = 1 / (1 + exp(-(tmp_sum_w + tmp_arbm->hbias[h_counter])));
			}


			//model項近似
			std::vector<GBRBM::VNODETYPE> vt = std::vector<GBRBM::VNODETYPE>(tmp_arbm->vlength, 0);
			std::vector<double> ht = std::vector<double>(tmp_arbm->hlength);
			this->contrastiveDivergence(tmp_arbm, this->cdk);

			//vtセット
			for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
			{
				vt[v_counter] = tmp_arbm->vnodes[v_counter];
			}

			//htセット
			for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
			{
				ht[h_counter] = tmp_arbm->hnodes[h_counter];
			}

			//勾配更新
			//vbias更新
			for (unsigned int v_counter = 0; v_counter < available_rbm->vlength; v_counter++) {
				double gradient = this->learningRate * (v0[v_counter] - vt[v_counter]);
				double new_vbias = available_rbm->vbias[v_counter] + gradient;
				available_rbm->vbias[v_counter] = new_vbias;
				//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < available_rbm->hlength; h_counter++) {
				double gradient = this->learningRate * (h0[h_counter] - ht[h_counter]);
				double new_hbias = available_rbm->hbias[h_counter] + gradient;
				available_rbm->hbias[h_counter] = new_hbias;
				//printf("hbias[%d]: %e\n", hnode_count, gradient);
			}

			//weight更新
			for (unsigned int v_count = 0; v_count < available_rbm->vlength; v_count++) {
				for (unsigned int h_count = 0; h_count < available_rbm->hlength; h_count++) {
					double gradient = this->learningRate * (v0[v_count] * h0[h_count] - vt[v_count] * ht[h_count]);
					double new_weight = available_rbm->weight[v_count][h_count] + gradient;
					available_rbm->weight[v_count][h_count] = new_weight;
					//printf("weight[%d][%d][%d]: %e\n", v_count, k_count, h_count, gradient);
				}
			}

			//variance更新
			for (unsigned int v_counter = 0; v_counter < available_rbm->vlength; v_counter++)
			{
				double gradient = this->learningRate * (pow((double)v0[v_counter] / (double)available_rbm->variance[v_counter], 2) / 2.0 - pow((double)vt[v_counter] / (double)available_rbm->variance[v_counter], 2) / 2.0);
				double new_variance = available_rbm->variance[v_counter] + gradient;
				available_rbm->variance[v_counter] = new_variance <= 0 ? available_rbm->variance[v_counter] : new_variance;
				//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
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

bool GBRBMCFTrain::trainBatch(GBRBMCF & rbm, GBRBMDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		auto gradient_vbias = std::vector<double>(this->vlength, 0);;
		auto gradient_variance = std::vector<double>(this->vlength, 0);;
		auto gradient_hbias = std::vector<double>(this->hlength, 0);;
		auto gradient_weight = std::vector< std::vector<double> >(this->vlength, std::vector<double>(this->hlength, 0));;

		this->_tmpGradientVbias = std::vector<double>(this->vlength, 0);
		this->_tmpGradientVariance = std::vector<double>(this->vlength, 0);
		this->_tmpGradientHbias = std::vector<double>(this->hlength, 0);
		this->_tmpGradientWeight = std::vector< std::vector<double> >(this->vlength, std::vector<double>(this->hlength, 0));
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
				gradient_vbias[vno] += this->_tmpGradientVbias[vno];
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				gradient_hbias[h_counter] += this->_tmpGradientHbias[h_counter];
			}

			//weight更新
			for (auto & vno : tmp_table)
			{
				for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
					gradient_weight[vno][h_count] += this->_tmpGradientWeight[vno][h_count];
				}
			}

			//variance更新
			for (auto & vno : tmp_table)
			{
				gradient_variance[vno] += this->_tmpGradientVariance[vno];
			}
		}

		//勾配更新
		//vbias更新
		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
			double gradient = this->learningRate * (gradient_vbias[v_counter] / (double)this->_table[v_counter]);
			double new_vbias = rbm.vbias[v_counter] + gradient;
			rbm.vbias[v_counter] = new_vbias;
		}

		//hbias更新
		for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
			double gradient = this->learningRate * (gradient_hbias[h_counter] / (double)datasets.getLength());
			double new_hbias = rbm.hbias[h_counter] + gradient;
			rbm.hbias[h_counter] = new_hbias;
		}

		//weight更新
		for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
			for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
				double gradient = this->learningRate * (gradient_weight[v_count][h_count] / (double)this->_table[v_count]);
				double new_weight = rbm.weight[v_count][h_count] + gradient;
				rbm.weight[v_count][h_count] = new_weight;
			}
		}

		//variance更新
		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
			double gradient = this->learningRate * (gradient_variance[v_counter] / (double)this->_table[v_counter]);
			if (abs(gradient) > 50) gradient = gradient / pow(10, ceil(log10(abs(gradient)))) / 10.0;
			double new_variance = rbm.variance[v_counter] + gradient;
			rbm.variance[v_counter] = new_variance <= 0 ? rbm.variance[v_counter] : new_variance;
		}
	}
	//カルバックライブラー情報量は?

	return true;
}

bool GBRBMCFTrain::_getGradient(GBRBMCF & rbm, GBRBMDataset & dataset)
{
	//有効可視変数のみのRBM
	GBRBMCFBuilder::setDataset(rbm, dataset);
	GBRBM *available_rbm = rbm.getAvailableRBM();
	GBRBM *tmp_arbm = GBRBMBuilder::duplicate(available_rbm);


	//data項
	std::vector<GBRBM::VNODETYPE> v0 = std::vector<GBRBM::VNODETYPE>(tmp_arbm->vlength, 0);
	std::vector<double> h0 = std::vector<double>(tmp_arbm->hlength);


	//v0セット
	for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
	{
		v0[v_counter] = tmp_arbm->vnodes[v_counter];
	}

	//h0セット
	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		double tmp_sum_w = 0;

		for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
		{
			tmp_sum_w += tmp_arbm->weight[v_counter][h_counter] * tmp_arbm->vnodes[v_counter];
		}

		h0[h_counter] = 1 / (1 + exp(-(tmp_sum_w + tmp_arbm->hbias[h_counter])));
	}


	//model項近似
	std::vector<GBRBM::VNODETYPE> vt = std::vector<GBRBM::VNODETYPE>(tmp_arbm->vlength, 0);
	std::vector<double> ht = std::vector<double>(tmp_arbm->hlength);
	this->contrastiveDivergence(tmp_arbm, this->cdk);

	//vtセット
	for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
	{
		vt[v_counter] = tmp_arbm->vnodes[v_counter];
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

		double gradient = (v0[v_counter] - vt[v_counter]);
		this->_tmpGradientVbias[vno] = gradient;
		//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
	}

	//hbias更新
	for (unsigned int h_counter = 0; h_counter < available_rbm->hlength; h_counter++) {
		double gradient = (h0[h_counter] - ht[h_counter]);
		this->_tmpGradientHbias[h_counter] = gradient;
		//printf("hbias[%d]: %e\n", hnode_count, gradient);
	}

	//weight更新
	for (unsigned int v_count = 0; v_count < available_rbm->vlength; v_count++)
	{
		unsigned int vno = rbm.getAvailableTable()[v_count];

		for (unsigned int h_count = 0; h_count < available_rbm->hlength; h_count++) {
			double gradient = (v0[v_count] * h0[h_count] - vt[v_count] * ht[h_count]);
			this->_tmpGradientWeight[vno][h_count] = gradient;
		}
	}

	//variance更新
	for (unsigned int v_counter = 0; v_counter < available_rbm->vlength; v_counter++) {

		unsigned int vno = rbm.getAvailableTable()[v_counter];

		double gradient = (pow((double)v0[v_counter] / (double)available_rbm->variance[v_counter], 2) / 2.0 - pow((double)vt[v_counter] / (double)available_rbm->variance[v_counter], 2) / 2.0);
		this->_tmpGradientVariance[vno] = gradient;
	}



	//後始末
	delete available_rbm;
	delete tmp_arbm;

	return true;
}

bool GBRBMCFTrain::contrastiveDivergence(GBRBM *rbm, unsigned int cdk)
{
	rbm->gibbsSampleHidden();
	rbm->gibbsSample(cdk);

	return true;
}
