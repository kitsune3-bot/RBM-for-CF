#include "GBRBMCF.h"
#include <random>

bool GBRBMCF::_create()
{
	this->_createVnodes();
	this->_createHnodes();
	this->_createVbias();
	this->_createHbias();
	this->_createWeight();
	this->_createVariance();

	return true;
}

bool GBRBMCF::_createVnodes()
{
	this->vnodes = std::vector<GBRBM::VNODETYPE>(this->vlength, 0);

	return true;
}

bool GBRBMCF::_createHnodes()
{
	this->hnodes = std::vector<GBRBM::HNODETYPE>(this->hlength, 0);

	return true;
}

bool GBRBMCF::_createVbias()
{
	this->vbias = std::vector<GBRBM::VBIASTYPE>(this->vlength, 0);

	return true;
}

bool GBRBMCF::_createHbias()
{
	this->hbias = std::vector<GBRBM::HBIASTYPE>(this->hlength, 0);

	return true;
}

bool GBRBMCF::_createWeight()
{
	this->weight = std::vector<std::vector<GBRBM::WEIGHTTYPE> >(this->vlength, std::vector<GBRBM::WEIGHTTYPE>(hlength, 0));

	return true;
}

bool GBRBMCF::_createVariance()
{
	this->variance = std::vector<GBRBM::VARIANCETYPE>(this->vlength, 1);

	return true;
}

bool GBRBMCF::_init()
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


GBRBMCF *GBRBMCF::_clone()
{
	GBRBMCF *rbm = new GBRBMCF(this->vlength, this->hlength);
	this->_copy(rbm);

	return rbm;
}

bool GBRBMCF::_copy(GBRBMCF *targetRBM)
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
		for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
		{
			targetRBM->weight[v_counter][h_counter] = this->weight[v_counter][h_counter];
		}
	}

	for (unsigned int h_counter = 0; h_counter<this->hlength; h_counter++)
	{
		targetRBM->hnodes[h_counter] = this->hnodes[h_counter];
		targetRBM->hbias[h_counter] = this->hbias[h_counter];
	}

	targetRBM->_table = this->getAvailableTable();

	return true;
}

GBRBMCF::GBRBMCF()
{
}


GBRBMCF::~GBRBMCF()
{
}

GBRBMCF::GBRBMCF(unsigned int vlength, unsigned int hlength)
{
	this->vlength = vlength;
	this->hlength = hlength;

	this->_create();
	this->_init();
}

GBRBM *GBRBMCF::getAvailableRBM()
{
	GBRBM *gbrbm = new GBRBM(this->getAvailableSize(), this->hlength);


	//VNODE SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		gbrbm->vnodes[v_counter] = this->vnodes[vno];
	}

	//HNODE SET
	for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
	{
		gbrbm->hnodes[h_counter] = this->hnodes[h_counter];
	}

	//VBIAS SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		gbrbm->vbias[v_counter] = this->vbias[vno];
	}

	//HBIAS SET
	for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
	{
		gbrbm->hbias[h_counter] = this->hbias[h_counter];
	}

	//WEIGHT SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
		{
			gbrbm->weight[v_counter][h_counter] = this->weight[vno][h_counter];
		}
	}

	//VARIANCE SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		gbrbm->variance[v_counter] = this->variance[vno];
	}


	return gbrbm;
}

bool GBRBMCF::setAvailableRBM(GBRBM *gbrbm)
{
	if (this->getAvailableSize() != gbrbm->vlength || this->hlength != gbrbm->hlength) return false;

	//VNODE SET
	//

	//HNODE SET
	//

	//VBIAS SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		this->vbias[vno] = gbrbm->vbias[v_counter];
	}

	//HBIAS SET
	for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
	{
		this->hbias[h_counter] = gbrbm->hbias[h_counter];
	}

	//WEIGHT SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
		{
			this->weight[vno][h_counter] = gbrbm->weight[v_counter][h_counter];
		}
	}

	//VARIANCE SET
	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		unsigned int vno = this->_table[v_counter];

		this->variance[vno] = gbrbm->variance[v_counter];
	}


	return true;
}

std::deque<unsigned int> GBRBMCF::getAvailableTable()
{
	//戻り値ポインタでなくてもいいの?
	return this->_table;
}

size_t GBRBMCF::getAvailableSize()
{
	return this->_table.size();
}

GBRBM::VNODETYPE GBRBMCF::predict(unsigned int vnodeno)
{
	//あってる保証まだできない
	if (vnodeno < 0 || this->vlength <= vnodeno) return 0;

	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);

	if (it != this->_table.end()) return this->vnodes[vnodeno];

	GBRBMCF *tmp_rbm = this->_clone();
	//tmp_rbm->_table.push_back(vnodeno);
	GBRBM *tmp_arbm = tmp_rbm->getAvailableRBM();

	//隠れ層確率
	std::vector<double> ph;
	ph.assign(tmp_arbm->hlength, 0);

	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		double tmp_sum = 0;

		for (unsigned int v_counter = 0; v_counter < tmp_arbm->vlength; v_counter++)
		{
			tmp_sum += tmp_arbm->vnodes[v_counter] * tmp_arbm->weight[v_counter][h_counter];
		}

		ph[h_counter] = 1 / (1 + exp(-(tmp_arbm->hbias[h_counter] + tmp_sum)));
	}

	//可視変数増やすのでお役御免, 
	delete tmp_arbm;


	//予想可視変数の確率
	tmp_rbm->_table.push_back(vnodeno);
	tmp_arbm = tmp_rbm->getAvailableRBM();
	unsigned int vno = tmp_arbm->vlength - 1;//これでいいのだろうか
	double sum_weight = 0;

	for (unsigned int h_counter = 0; h_counter < tmp_arbm->hlength; h_counter++)
	{
		sum_weight += tmp_arbm->weight[vno][h_counter] * (double)ph[h_counter];
	}

	double mu = tmp_arbm->variance[vno] * (tmp_arbm->vbias[vno] + sum_weight);
	
	delete tmp_arbm;
	delete tmp_rbm;

	return mu;

}

bool GBRBMCF::disableVnode(unsigned int vnodeno)
{
	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);

	while (it != this->_table.end())
	{
		this->_table.erase(it);
		//この検索法だと無駄多い(前半部分が何度も重複検索してる)
		it = std::find(this->_table.begin(), this->_table.end(), vnodeno);
	}
	return true;
}

bool GBRBMCF::disableVnodeAll()
{
	this->_table.clear();

	return true;
}

bool GBRBMCF::enableVnode(unsigned int vnodeno)
{
	std::deque<unsigned int>::iterator it;
	it = std::find(this->_table.begin(), this->_table.end(), vnodeno);
	if ( it == this->_table.end() ) {
		this->_table.push_back(vnodeno);
	}

	return true;
}

bool GBRBMCF::enableVnodeAll()
{
	this->_table.clear();
	for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
	{
		this->_table.push_back(v_counter);
	}

	return true;
}

GBRBM * GBRBMCF::getMeanFieldGBRBM()
{
	unsigned int n = 100;

	GBRBMCF *rbm = this->_clone();
	rbm->meanFieldUpdate();

	return (GBRBM *)rbm;
}

bool GBRBMCF::meanFieldUpdate()
{
	unsigned int n = 150;

	for (unsigned int i = 0; i < n; i++)
	{
		for (unsigned int v_counter = 0; v_counter < this->vlength; v_counter++)
		{
			std::deque<unsigned int>::iterator it;
			it = std::find(this->_table.begin(), this->_table.end(), v_counter);

			if (it == this->_table.end()) {
				this->meanFieldUpdateVisible(v_counter);
			}
		}

		for (unsigned int h_counter = 0; h_counter < this->hlength; h_counter++)
		{
			this->meanFieldUpdateHidden(h_counter);
		}
	}

	return true;
}