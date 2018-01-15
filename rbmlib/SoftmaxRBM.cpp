#include "SoftmaxRBM.h"
#include "SoftmaxDataset.h"
#include "SoftmaxDatasetContainer.h"
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <algorithm>

SoftmaxRBM *SoftmaxRBM::_clone()
{
	SoftmaxRBM *rbm = new SoftmaxRBM(this->vlength, this->hlength, this->k);
	this->_copy(rbm);

	return rbm;
}

bool SoftmaxRBM::_copy(SoftmaxRBM *targetRBM)
{
	unsigned int v_counter;
	unsigned int k_counter;
	unsigned int h_counter;

	if (this->vlength != targetRBM->vlength ||
		this->k != targetRBM->k ||
		this->hlength != targetRBM->hlength) {
		return false;
	}

	for (v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		for (k_counter = 0; k_counter < this->k; k_counter++)
		{
			targetRBM->vnodes[v_counter][k_counter] = this->vnodes[v_counter][k_counter];
			targetRBM->vbias[v_counter][k_counter] = this->vbias[v_counter][k_counter];
			for (h_counter = 0; h_counter<this->hlength; h_counter++)
			{
				targetRBM->weight[v_counter][k_counter][h_counter] = this->weight[v_counter][k_counter][h_counter];
			}
		}
	}

	for (h_counter = 0; h_counter<this->hlength; h_counter++)
	{
		targetRBM->hnodes[h_counter] = this->hnodes[h_counter];
		targetRBM->hbias[h_counter] = this->hbias[h_counter];
	}

	return true;
}

bool SoftmaxRBM::_create()
{
	this->_createVnodes();
	this->_createHnodes();
	this->_createVbias();
	this->_createHbias();
	this->_createWeight();

	return true;
}

bool SoftmaxRBM::_createVnodes()
{
	this->vnodes = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(vlength, std::vector<SoftmaxRBM::VNODETYPE>(k, 0));
	return true;
}

bool SoftmaxRBM::_createHnodes()
{
	this->hnodes = std::vector<SoftmaxRBM::HNODETYPE>(hlength, 0);
	return true;
}

bool SoftmaxRBM::_createVbias()
{
	this->vbias = std::vector<std::vector<SoftmaxRBM::VBIASTYPE> >(vlength, std::vector<SoftmaxRBM::VBIASTYPE>(k, 0));
	return true;
}

bool SoftmaxRBM::_createHbias()
{
	this->hbias = std::vector<SoftmaxRBM::HBIASTYPE>(hlength, 0);
	return true;
}

bool SoftmaxRBM::_createWeight()
{
	this->weight = std::vector<std::vector< std::vector<SoftmaxRBM::WEIGHTTYPE> > >(vlength, std::vector<std::vector<SoftmaxRBM::WEIGHTTYPE> >(k, std::vector<SoftmaxRBM::WEIGHTTYPE>(hlength, 0)));
	return true;
}

bool SoftmaxRBM::_init()
{
	unsigned int v_counter;
	unsigned int k_counter;
	unsigned int h_counter;

	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::uniform_real_distribution<double> dist(-1, 1);
	/*
	* initialize vbias
	*/
	for (v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		for (k_counter = 0; k_counter < this->k; k_counter++)
		{
			this->vbias[v_counter][k_counter] = dist(engine);
		}
	}

	/*
	* initialize hbias
	*/
	for (h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		//rand.h
		this->hbias[h_counter] = dist(engine);
		//softmaxrbm->hbias[h_counter] = 1;
	}

	/*
	* initialize weight
	*/
	for (v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		for (k_counter = 0; k_counter < this->k; k_counter++)
		{
			for (h_counter = 0; h_counter < this->hlength; h_counter++)
			{
				//rand.h
				this->weight[v_counter][k_counter][h_counter] = dist(engine);
			}
		}
	}

	return true;
}


double SoftmaxRBM::_getSumWeightHidden(unsigned int vindex, unsigned int kindex)
{
	double sum_weight = 0;
	for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		sum_weight += this->weight[vindex][kindex][h_counter] * (double) this->hnodes[h_counter];
	}

	return sum_weight;

}

double SoftmaxRBM::_getSumWeightVisible(unsigned int hindex)
{
	double sum_weight = 0;
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < this->k; k_counter++)
		{
			sum_weight += this->weight[v_counter][k_counter][hindex] * (double) this->vnodes[v_counter][k_counter];
		}
	}

	return sum_weight;
}


double SoftmaxRBM::_sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

SoftmaxRBM::SoftmaxRBM() { };

SoftmaxRBM::SoftmaxRBM(unsigned int vlength, unsigned int hlength, unsigned int k)
{
	this->vlength = vlength;
	this->k = k;
	this->hlength = hlength;

	this->_create();
	this->_init();
}



SoftmaxRBM::~SoftmaxRBM() { };


double SoftmaxRBM::conditionalProbabilityVisible(unsigned int vindex, unsigned int kindex)
{

	double denominator = 0;

	double value = 0;
	double ex = 0;
	double ex_under = 0;

	ex = exp(this->vbias[vindex][kindex] + this->_getSumWeightHidden(vindex, kindex));

	for (unsigned int k_counter = 0; k_counter<this->k; k_counter++)
	{
		ex_under = exp(this->vbias[vindex][k_counter] + this->_getSumWeightHidden(vindex, k_counter));
		
		denominator += ex_under;
	}

	value = ex / denominator;

	return value;

}

double SoftmaxRBM::conditionalProbabilityHidden(unsigned int index)
{

	double sum_weight = this->_getSumWeightVisible(index);

	double value = 1.0 / (1.0 + exp(-(this->hbias[index] + sum_weight)));

	return value;
}


bool SoftmaxRBM::gibbsSample(unsigned int n)
{
	unsigned int v_counter;

	for (v_counter = 0; v_counter < n; v_counter++)
	{
		this->gibbsSampleVisible();
		this->gibbsSampleHidden();
	}

	return true;
}

bool SoftmaxRBM::gibbsSampleVisible()
{
	SoftmaxRBM::VNODETYPE value;

	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::uniform_real_distribution<double> dist(0.0, 1.0);


	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		double under = 0;
		for (unsigned int k_counter = 0; k_counter<this->k; k_counter++)
		{
			under += this->conditionalProbabilityVisible(v_counter, k_counter);
		}

		double p = dist(engine);
		double p_min = 0;
		double p_max = 0;

		for (unsigned int k_counter = 0; k_counter<this->k; k_counter++)
		{
			p_min = p_max;
			p_max += this->conditionalProbabilityVisible(v_counter, k_counter) / under;

			value = (p_min <= p && p < p_max) ? 1 : 0;
			this->vnodes[v_counter][k_counter] = value;
		}
	}

	return true;
}

bool SoftmaxRBM::gibbsSampleHidden()
{
	unsigned int h_counter;
	bool value;
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());

	for (h_counter = 0; h_counter < this->hlength; h_counter++)
	{
		std::bernoulli_distribution dist(this->conditionalProbabilityHidden(h_counter));
		value = dist(engine);
		this->hnodes[h_counter] = value == true ? 1 : 0;
	}

	return true;
}

int SoftmaxRBM::getVnodeValue(unsigned int vindex)
{

	//error check
	if (this->vlength < vindex) return 0;
	
	std::vector<double>::iterator iter = std::max_element(this->vnodes[vindex].begin(), this->vnodes[vindex].end());
	size_t index = std::distance(this->vnodes[vindex].begin(), iter);

	unsigned int value = (unsigned int)index + 1;

	return value;
}

bool SoftmaxRBM::setVnodeValue(unsigned int vindex, int value)
{
	double diff = 0.0001;

	//error check
	if (this->vlength < vindex) return false;

	//set value
	for (unsigned int k_counter = 0; k_counter < this->k; k_counter++)
	{
		this->vnodes[vindex][k_counter] = ( fabs((k_counter + 1) - (double)value) < diff) ? 1 : 0;
	}

	return true;
}

unsigned int SoftmaxRBM::reconstructVnode(unsigned int vindex)
{
	if (vindex < 0 || this->vlength <= vindex) return 0;

	SoftmaxRBM *tmp_rbm = this->_clone();

	//隠れ層確率
	std::vector<double> ph(tmp_rbm->hlength, 0);
	//ph.assign(tmp_arbm->hlength, 0);

	for (unsigned int h_counter = 0; h_counter < tmp_rbm->hlength; h_counter++)
	{
		double sum_weight = tmp_rbm->_getSumWeightVisible(h_counter);
		ph[h_counter] = tmp_rbm->_sigmoid(tmp_rbm->hbias[h_counter] + sum_weight);
	}



	//予想可視1-of-k符号の確率
	std::vector<double> pv(tmp_rbm->k, 0);

	//よくよく考えたら分母をわざわざ計算する必要なかった…
	for (unsigned int k_counter = 0; k_counter < tmp_rbm->k; k_counter++)
	{
		double tmp_sum = 0;
		unsigned int vno = vindex;//これでいいのだろうか

		for (unsigned int h_counter = 0; h_counter < tmp_rbm->hlength; h_counter++)
		{
			tmp_sum += ph[h_counter] * tmp_rbm->weight[vno][k_counter][h_counter];
		}

		pv[k_counter] = exp(tmp_sum + tmp_rbm->vbias[vno][k_counter]);
	}


	//一番確率高いの選ぶ
	//unsigned int value = 0;
	//for (unsigned int k_counter = 0; k_counter < tmp_rbm->k; k_counter++)
	//{
	//	value = pv[value] < pv[k_counter] ? k_counter : value;
	//}
	std::vector<double>::iterator iter = std::max_element(pv.begin(), pv.end());
	size_t index = std::distance(pv.begin(), iter);

	unsigned int value = (unsigned int)index + 1;

	delete tmp_rbm;

	return value;
}

bool SoftmaxRBM::meanFieldUpdateVisible(unsigned int vindex)
{
	for (unsigned int k_counter = 0; k_counter < this->k; k_counter++)
	{
		double new_value = this->conditionalProbabilityVisible(vindex, k_counter);
		this->vnodes[vindex][k_counter] = new_value;
	}

	return true;
}

bool SoftmaxRBM::meanFieldUpdateHidden(unsigned int hindex)
{
	double new_value = this->conditionalProbabilityHidden(hindex);
	this->hnodes[hindex] = new_value;

	return true;
}