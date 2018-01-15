#include "GBRBMTrain.h"

GBRBMTrain::GBRBMTrain(unsigned int vlen, unsigned int hlen, unsigned int epoch, double leaning_rate)
{
	this->vlength = vlen;
	this->hlength = hlen;
	this->epoch = epoch;
	this->learningRate = leaning_rate;
	this->_tmpGradientVbias = std::vector<double>(vlen, 0);
	this->_tmpGradientVariance = std::vector<double>(vlen, 0);
	this->_tmpGradientHbias = std::vector<double>(hlen, 0);
	this->_tmpGradientWeight = std::vector< std::vector<double> >(vlen, std::vector<double>(hlen, 0));
}

GBRBMTrain::GBRBMTrain(unsigned int vlen, unsigned int hlen, unsigned int epoch, double leaning_rate, unsigned int cdk)
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
}


GBRBMTrain::~GBRBMTrain()
{
}

bool GBRBMTrain::train(GBRBM & rbm, GBRBMDatasetContainer & datasets)
{
	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{

			this->_getGradient(rbm, datasets[dat_counter]);

			double lr = this->learningRate;

			//勾配更新
			//vbias更新
			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				double gradient = lr * this->_tmpGradientVbias[v_counter];
				if (abs(gradient) > 10) gradient = gradient / pow(10, ceil(log10(abs(gradient))));
				double new_vbias = rbm.vbias[v_counter] + gradient;
				rbm.vbias[v_counter] = new_vbias;
				//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				double gradient = lr * this->_tmpGradientHbias[h_counter];
				double new_hbias = rbm.hbias[h_counter] + gradient;
				rbm.hbias[h_counter] = new_hbias;
			}

			//weight更新
			for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
				for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
					double gradient = lr * this->_tmpGradientWeight[v_count][h_count];
					double new_weight = rbm.weight[v_count][h_count] + gradient;
					rbm.weight[v_count][h_count] = new_weight;
				}
			}

			//variance更新
			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				double gradient = lr * this->_tmpGradientVariance[v_counter];
				if (abs(gradient) > 10) gradient = gradient / pow(10, ceil(log10(abs(gradient))));
				double new_variance = rbm.variance[v_counter] + gradient;
				rbm.variance[v_counter] = new_variance <= 0 ? rbm.variance[v_counter] : new_variance;
			}
		}
	}
	//カルバックライブラー情報量は?

	return true;
}

bool GBRBMTrain::trainBatch(GBRBM & rbm, GBRBMDatasetContainer & datasets)
{

	auto gradientVbias = std::vector<double>(rbm.vlength, 0);
	auto gradientVariance = std::vector<double>(rbm.vlength, 0);
	auto gradientHbias = std::vector<double>(rbm.hlength, 0);
	auto gradientWeight = std::vector< std::vector<double> >(rbm.vlength, std::vector<double>(rbm.hlength, 0));


	for (unsigned int epoch_counter = 0; epoch_counter < this->epoch; epoch_counter++)
	{
		for (unsigned int dat_counter = 0; dat_counter < datasets.getLength(); dat_counter++)
		{
			this->_getGradient(rbm, datasets[dat_counter]);
			//勾配平均とる

			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				gradientVbias[v_counter] += this->_tmpGradientVbias[v_counter] / (double)datasets.getLength();
			}

			//hbias更新
			for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
				gradientHbias[h_counter] += this->_tmpGradientHbias[h_counter] / (double)datasets.getLength();
			}

			//weight更新
			for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
				for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
					gradientWeight[v_count][h_count] += this->_tmpGradientWeight[v_count][h_count] / (double)datasets.getLength();
				}
			}

			//variance更新
			for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
				gradientVariance[v_counter] += this->_tmpGradientVariance[v_counter] / (double)datasets.getLength();
			}
		}
	}

	double lr = this->learningRate;

	//勾配更新
	//vbias更新
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
		double gradient = lr * gradientVbias[v_counter];
		if (abs(gradient) > 10) gradient = gradient / pow(10, ceil(log10(abs(gradient))));
		double new_vbias = rbm.vbias[v_counter] + gradient;
		rbm.vbias[v_counter] = new_vbias;
		//printf("vbias[%d][%d]: %e\n", vnode_count, softmax_count, gradient);
	}

	//hbias更新
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
		double gradient = lr * gradientHbias[h_counter];
		double new_hbias = rbm.hbias[h_counter] + gradient;
		rbm.hbias[h_counter] = new_hbias;
	}

	//weight更新
	for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
		for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
			double gradient = lr * gradientWeight[v_count][h_count];
			double new_weight = rbm.weight[v_count][h_count] + gradient;
			rbm.weight[v_count][h_count] = new_weight;
		}
	}

	//variance更新
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
		double gradient = lr * gradientVariance[v_counter];
		if (abs(gradient) > 10) gradient = gradient / pow(10, ceil(log10(abs(gradient))));
		double new_variance = rbm.variance[v_counter] + gradient;
		rbm.variance[v_counter] = new_variance <= 0 ? rbm.variance[v_counter] : new_variance;
	}

	return true;
}


bool GBRBMTrain::_getGradient(GBRBM & rbm, GBRBMDataset & dataset)
{
	//data項
	std::vector<double> v0 = std::vector<double>(rbm.vlength, 0);
	std::vector<double> h0 = std::vector<double>(rbm.hlength, 0);

	//model項近似
	std::vector<double> vt = std::vector<double>(rbm.vlength, 0);
	std::vector<double> ht = std::vector<double>(rbm.hlength, 0);

	double lr = this->learningRate;

	//RBM SETDATASET
	GBRBMBuilder::setDataset(rbm, dataset);


	//v0セット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		v0[v_counter] = rbm.vnodes[v_counter];
	}

	//h0セット
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++)
	{
		double tmp_sum_w = 0;

		for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
		{
			tmp_sum_w += rbm.weight[v_counter][h_counter] * v0[v_counter];
		}

		h0[h_counter] = 1 / (1 + exp(-(tmp_sum_w + rbm.hbias[h_counter])));
	}


	this->_contrastiveDivergence( rbm, this->cdk);
	//vtセット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		vt[v_counter] = rbm.vnodes[v_counter];
	}

	//htセット
	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++)
	{
		ht[h_counter] = rbm.hnodes[h_counter];
	}

	//勾配セット
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
		double gradient =  (v0[v_counter] - vt[v_counter]);
		this->_tmpGradientVbias[v_counter] = gradient;
	}

	for (unsigned int h_counter = 0; h_counter < rbm.hlength; h_counter++) {
		double gradient = h0[h_counter] - ht[h_counter];
		this->_tmpGradientHbias[h_counter] = gradient;
	}

	for (unsigned int v_count = 0; v_count < rbm.vlength; v_count++) {
		for (unsigned int h_count = 0; h_count < rbm.hlength; h_count++) {
			double gradient = (v0[v_count] * h0[h_count] - vt[v_count] * ht[h_count]);
			this->_tmpGradientWeight[v_count][h_count] = gradient;
		}
	}

	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++) {
		double gradient = (pow((double)v0[v_counter] / (double)rbm.variance[v_counter], 2) / 2.0 - pow((double)vt[v_counter] / (double)rbm.variance[v_counter], 2) / 2.0);
		this->_tmpGradientVariance[v_counter] = gradient;
	}

	return true;
}

bool GBRBMTrain::_contrastiveDivergence(GBRBM &rbm, unsigned int cdk)
{
	rbm.gibbsSampleHidden();
	rbm.gibbsSample(cdk);

	return true;
}
