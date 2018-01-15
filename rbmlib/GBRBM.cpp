#include "GBRBM.h"
#include <random>
#include <cmath>
#include <fstream>

GBRBM *GBRBM::_clone()
{
	GBRBM *rbm = new GBRBM(this->vlength, this->hlength);
	this->_copy(rbm);

	return rbm;
}

bool GBRBM::_copy(GBRBM *targetRBM)
{
	if (this->vlength != targetRBM->vlength ||
		this->hlength != targetRBM->hlength) {
		return false;
	}

	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		targetRBM->vnodes[v_counter] = this->vnodes[v_counter];
		targetRBM->vbias[v_counter] = this->vbias[v_counter];
		targetRBM->variance[v_counter] = this->variance[v_counter];
		for (unsigned int h_counter = 0; h_counter<this->hlength; h_counter++)
		{
			targetRBM->weight[v_counter][h_counter] = this->weight[v_counter][h_counter];
		}
	}

	for (unsigned int h_counter = 0; h_counter<this->hlength; h_counter++)
	{
		targetRBM->hnodes[h_counter] = this->hnodes[h_counter];
		targetRBM->hbias[h_counter] = this->hbias[h_counter];
	}

	return true;
}

bool GBRBM::_create()
{
	this->_createVnodes();
	this->_createHnodes();
	this->_createVbias();
	this->_createHbias();
	this->_createWeight();
	this->_createVariance();

	return true;
}

bool GBRBM::_createVnodes()
{
	this->vnodes = std::vector<GBRBM::VNODETYPE>(this->vlength, 0);

	return true;
}

bool GBRBM::_createHnodes()
{
	this->hnodes = std::vector<GBRBM::HNODETYPE>(this->hlength, 0);

	return true;
}

bool GBRBM::_createVbias()
{
	this->vbias = std::vector<GBRBM::VBIASTYPE>(this->vlength, 0);

	return true;
}

bool GBRBM::_createHbias()
{
	this->hbias = std::vector<GBRBM::HBIASTYPE>(this->hlength, 0);

	return true;
}

bool GBRBM::_createWeight()
{
	this->weight = std::vector<std::vector<GBRBM::WEIGHTTYPE> >(this->vlength, std::vector<GBRBM::WEIGHTTYPE>(hlength, 0));

	return true;
}

bool GBRBM::_createVariance()
{
	this->variance = std::vector<GBRBM::VARIANCETYPE>(this->vlength, 1);

	return true;
}

bool GBRBM::_init()
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::uniform_real_distribution<double> dist(-0.01, 0.01);
	/*
	* initialize vbias
	*/
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		this->vbias[v_counter] = dist(engine);
		this->variance[v_counter] = 1;
	}

	/*
	* initialize vbias
	*/
	for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		this->hbias[h_counter] = dist(engine);
	}

	/*
	* initialize weight
	*/
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
		{
			//rand.h
			this->weight[v_counter][h_counter] = dist(engine);
		}
	}

	return true;
}


double GBRBM::_getSumWeightHidden(unsigned int vindex)
{
	double sum_weight = 0;
	for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		sum_weight += this->weight[vindex][h_counter] * (double) this->hnodes[h_counter];
	}

	return sum_weight;
}

double GBRBM::_getSumWeightVisible(unsigned int hindex)
{
	double sum_weight = 0;
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		sum_weight += this->weight[v_counter][hindex] * (double) this->vnodes[v_counter];
	}

	return sum_weight;
}

double GBRBM::_sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

double GBRBM::_gaussianDist(double x, double mu, double sigma2)
{
	return exp(-pow(x - mu, 2) / (2 * sigma2)) / sqrt(2 * 3.14159265358979 * sigma2);
}

double GBRBM::_mu(unsigned int vindex)
{

	double sum_wh = this->_getSumWeightHidden(vindex);

	double mu = this->variance[vindex] * (this->vbias[vindex] + sum_wh);

	return mu;
}

double GBRBM::_lambda(unsigned int hindex)
{
	double sum_wv = this->_getSumWeightVisible(hindex);

	double lambda = this->hbias[hindex] + sum_wv;

	return lambda;
}



GBRBM::GBRBM()
{
}

GBRBM::GBRBM(unsigned int vlength, unsigned int hlength)
{
	this->vlength = vlength;
	this->hlength = hlength;

	this->_create();
	this->_init();
}


GBRBM::~GBRBM()
{
}


double GBRBM::conditionalProbabilityVisible(unsigned int vindex)
{
	double sum_weight = this->_getSumWeightHidden(vindex);

	double mu = this->_mu(vindex);

	double probability = this->_gaussianDist(this->vnodes[vindex], mu, this->variance[vindex]);

	return probability;
}


double GBRBM::conditionalProbabilityHidden(unsigned int hindex)
{
	double sum_weight = this->_getSumWeightVisible(hindex);

	double probability = this->_sigmoid(this->hbias[hindex] + sum_weight);

	bool nan_chk = std::isnan(probability);

	return probability;
}

bool GBRBM::gibbsSample(unsigned int n)
{
	unsigned int v_counter;

	for (v_counter = 0; v_counter < n; v_counter++)
	{
		this->gibbsSampleVisible();
		this->gibbsSampleHidden();
	}

	return true;
}

bool GBRBM::gibbsSampleVisible()
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());


	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		double mu = this->_mu(v_counter);

		std::normal_distribution<> dist(mu, sqrt(this->variance[v_counter]));
		double new_value = dist(engine);
		this->vnodes[v_counter] = new_value;
	}

	return true;
}

bool GBRBM::gibbsSampleHidden()
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());

	for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		GBRBM::HNODETYPE value;

		double ph = this->conditionalProbabilityHidden(h_counter);
		std::bernoulli_distribution dist(ph);
		value = dist(engine);
		this->hnodes[h_counter] = value ? 1 : 0;
	}

	return true;
}

double GBRBM::getVnodeValue(unsigned int vindex)
{
	return this->vnodes[vindex];
}

bool GBRBM::setVnodeValue(unsigned int vindex, double value)
{
	this->vnodes[vindex] = value;

	return true;
}


double GBRBM::reconstructVnode(unsigned int vindex)
{
	if (vindex < 0 || this->vlength <= vindex) return 0;

	double value = 0;

	GBRBM *tmp_rbm = this->_clone();
	
	//隠れ層変数期待値
	std::vector<double> ph(tmp_rbm->hlength, 0);
	//ph.assign(tmp_arbm->hlength, 0);
	

	for (unsigned int h_counter = 0; h_counter < tmp_rbm->hlength; h_counter++)
	{
		double sum_weight_hidden = this->_getSumWeightVisible(h_counter);
		ph[h_counter] = this->_sigmoid(tmp_rbm->hbias[h_counter] + sum_weight_hidden);
	}

	//これ予想値って平均値なるんじゃ
	{
		//double mu = this->variance[vindex] * (this->_getSumWeightHidden(vindex) + this->vbias[vindex]);
		double sum_w = 0;
		for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
		{
			sum_w += this->weight[vindex][h_counter] * ph[h_counter];
		}
		double mu = this->variance[vindex] * (sum_w + this->vbias[vindex]);
		value = mu;
	}

	delete tmp_rbm;

	return value;
	//return this->_gaussianDist(this->vnodes[vindex], value, this->variance[vindex]) * this->vnodes[vindex];
}

bool GBRBM::meanFieldUpdateVisible(unsigned int vindex)
{
	GBRBM::VNODETYPE mu = this->_mu(vindex);
	this->vnodes[vindex] = mu;

	return true;
}

bool GBRBM::meanFieldUpdateHidden(unsigned int hindex)
{
	double sum_w = this->_getSumWeightVisible(hindex);
	double lambda = this->_lambda(hindex);

	double tmp_w2 = 0.0;
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		tmp_w2 = pow(this->weight[v_counter][hindex], 2) * (1 - 2 * this->hnodes[hindex]) ;
	}
	double new_value = this->_sigmoid(lambda + tmp_w2 / 2);

	this->hnodes[hindex] = new_value;

	return true;
}

