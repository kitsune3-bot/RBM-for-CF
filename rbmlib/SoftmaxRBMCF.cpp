#include "SoftmaxRBMCF.h"
#include <deque>
#include <algorithm>
#include <random>

bool SoftmaxRBMCF::_create()
{
	this->_createVnodes();
	this->_createHnodes();
	this->_createVbias();
	this->_createHbias();
	this->_createWeight();

	return true;
}

bool SoftmaxRBMCF::_createVnodes()
{
	this->vnodes = std::vector<std::vector<SoftmaxRBM::VNODETYPE> >(vlength, std::vector<SoftmaxRBM::VNODETYPE>(k, 0));
	return true;
}

bool SoftmaxRBMCF::_createHnodes()
{
	this->hnodes = std::vector<SoftmaxRBM::HNODETYPE>(hlength, 0);
	return true;
}

bool SoftmaxRBMCF::_createVbias()
{
	this->vbias = std::vector<std::vector<SoftmaxRBM::VBIASTYPE> >(vlength, std::vector<SoftmaxRBM::VBIASTYPE>(k, 0));
	return true;
}

bool SoftmaxRBMCF::_createHbias()
{
	this->hbias = std::vector<SoftmaxRBM::HBIASTYPE>(hlength, 0);
	return true;
}

bool SoftmaxRBMCF::_createWeight()
{
	this->weight = std::vector<std::vector< std::vector<SoftmaxRBM::WEIGHTTYPE> > >(vlength, std::vector<std::vector<SoftmaxRBM::WEIGHTTYPE> >(k, std::vector<SoftmaxRBM::WEIGHTTYPE>(hlength, 0)));
	return true;
}

bool SoftmaxRBMCF::_init()
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

SoftmaxRBMCF *SoftmaxRBMCF::_clone()
{
	SoftmaxRBMCF *rbm = new SoftmaxRBMCF(this->vlength, this->hlength, this->k);
	this->_copy(rbm);

	return rbm;
}

bool SoftmaxRBMCF::_copy(SoftmaxRBMCF *targetRBM)
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

	targetRBM->_table = this->getAvailableTable();

	return true;
}


SoftmaxRBMCF::SoftmaxRBMCF()
{
}


SoftmaxRBMCF::~SoftmaxRBMCF()
{
}

SoftmaxRBMCF::SoftmaxRBMCF(unsigned int vlength, unsigned int hlength, unsigned int k)
{
	this->vlength = vlength;
	this->k = k;
	this->hlength = hlength;

	this->_create();
	this->_init();
}

SoftmaxRBM *SoftmaxRBMCF::getAvailableRBM()
{
	SoftmaxRBM *srbm = new SoftmaxRBM(this->getAvailableSize(), this->hlength, this->k);


	//VNODE SET
	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			srbm->vnodes[v_counter][k_counter] = this->vnodes[vno][k_counter];
		}
	}

	//HNODE SET
	for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
	{
		srbm->hnodes[h_counter] = this->hnodes[h_counter];
	}

	//VBIAS SET
	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			srbm->vbias[v_counter][k_counter] = this->vbias[vno][k_counter];
		}
	}

	//HBIAS SET
	for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
	{
		srbm->hbias[h_counter] = this->hbias[h_counter];
	}

	//WEIGHT SET
	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
			{
				srbm->weight[v_counter][k_counter][h_counter] = this->weight[vno][k_counter][h_counter];
			}
		}
	}

	return srbm;
}

bool SoftmaxRBMCF::setAvailableRBM(SoftmaxRBM * srbm)
{
	if (this->getAvailableSize() != srbm->vlength || this->hlength != srbm->hlength || this->k != srbm->k) return false;

	//VNODE SET
	//

	//HNODE SET
	//

	//VBIAS SET
	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];
		
		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			this->vbias[vno][k_counter] = srbm->vbias[v_counter][k_counter];
		}
	}

	//HBIAS SET
	for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
	{
		this->hbias[h_counter] = srbm->hbias[h_counter];
	}

	//WEIGHT SET
	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
			{
				this->weight[vno][k_counter][h_counter] = srbm->weight[v_counter][k_counter][h_counter];
			}
		}
	}

	return true;
}

std::deque<unsigned int> SoftmaxRBMCF::getAvailableTable()
{
	//戻り値ポインタでなくてもいいの?
	return this->_table;
}

size_t SoftmaxRBMCF::getAvailableSize()
{
	return this->_table.size();
}

unsigned int SoftmaxRBMCF::predict(unsigned int vnodeno)
{

	if (vnodeno < 0 || this->vlength <= vnodeno) return 0;

	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);

	if (it != this->_table.end()) return this->getVnodeValue(vnodeno);

	SoftmaxRBMCF *tmp_rbm = this->_clone();
	//tmp_rbm->_table.push_back(vnodeno);
	SoftmaxRBM *tmp_arbm = tmp_rbm->getAvailableRBM();

	//隠れ層確率
	std::vector<double> ph;
	ph.assign(tmp_arbm->hlength, 0);

	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		double tmp_sum = 0;

		for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
		{
			for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
			{
				tmp_sum += tmp_arbm->vnodes[v_counter][k_counter] * tmp_arbm->weight[v_counter][k_counter][h_counter];
			}
		}

		ph[h_counter] = 1.0 / (1.0 + exp(-(tmp_arbm->hbias[h_counter] + tmp_sum)));
	}

	//可視変数増やすのでお役御免, 
	delete tmp_arbm;


	//予想可視変数の確率
	tmp_rbm->_table.push_back(vnodeno);
	tmp_arbm = tmp_rbm->getAvailableRBM();
	std::vector<double> pv;
	pv.assign(tmp_arbm->k, 0);

	//よくよく考えたら分母をわざわざ計算する必要なかった…
	for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
	{
		double tmp_sum = 0;
		unsigned int vno = tmp_arbm->vlength - 1;//これでいいのだろうか

		for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
		{
			tmp_sum += ph[h_counter] * tmp_arbm->weight[vno][k_counter][h_counter];
		}

		pv[k_counter] =exp(tmp_sum + tmp_arbm->vbias[vno][k_counter]);
	}


	//一番確率高いの選ぶ
	//unsigned int value = 0;
	//for (unsigned int k_counter = 0; k_counter < tmp_arbm->k; k_counter++)
	//{
	//	value = pv[value] < pv[k_counter] ? k_counter : value;
	//}

	std::vector<double>::iterator iter = std::max_element(pv.begin(), pv.end());
	size_t index = std::distance(pv.begin(), iter);
	unsigned int value = index + 1;

	delete tmp_arbm;
	delete tmp_rbm;

	return value;
	
}

bool SoftmaxRBMCF::disableVnode(unsigned int vnodeno)
{
	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);
	this->_table.erase(it);

	return true;
}

bool SoftmaxRBMCF::disableVnodeAll()
{
	this->_table.clear();

	return true;
}

bool SoftmaxRBMCF::enableVnode(unsigned int vnodeno)
{
	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);
	if (it == this->_table.end()) {
		this->_table.push_back(vnodeno);
	}

	return true;
}

bool SoftmaxRBMCF::enableVnodeAll()
{
	this->_table.clear();
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		this->_table.push_back(v_counter);
	}

	return true;
}