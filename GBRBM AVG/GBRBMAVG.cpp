#include "GBRBMAVG.h"


GBRBMAVG::GBRBMAVG(unsigned int vlength, unsigned int hlength)
{
	this->_rbm = new GBRBMCF(vlength, hlength);
	this->_vsum.assign(this->_rbm->vlength, 0);

	//vbias
	for (unsigned int v_counter = 0; v_counter < this->_rbm->vlength; v_counter++)
	{
		this->_rbm->vbias[v_counter] = 0;
	}

	//hbias
	for (unsigned int h_counter = 0; h_counter < this->_rbm->hlength; h_counter++)
	{
		this->_rbm->hbias[h_counter] = 0;
	}


	//weight
	for (unsigned int v_counter = 0; v_counter < this->_rbm->vlength; v_counter++)
	{
		for (unsigned int h_counter = 0; h_counter < this->_rbm->hlength; h_counter++)
		{
			this->_rbm->weight[v_counter][h_counter] = 0;
		}
	}

	//variance
	for (unsigned int v_counter = 0; v_counter < this->_rbm->vlength; v_counter++)
	{
		this->_rbm->vbias[v_counter] = 0;
	}


}


GBRBMAVG::~GBRBMAVG()
{
	delete this->_rbm;
}

bool GBRBMAVG::addParameters(GBRBMCF *rbm)
{
	if (this->_rbm->vlength != rbm->vlength ||
		this->_rbm->hlength != rbm->hlength) {

		return false;
	}

	//vbias
	for (unsigned int v_counter = 0; v_counter < rbm->getAvailableSize(); v_counter++)
	{
		unsigned int vno = rbm->getAvailableTable()[v_counter];

		this->_rbm->vbias[vno] += rbm->vbias[vno];
	}

	//hbias
	for (unsigned int h_counter = 0; h_counter < rbm->hlength; h_counter++)
	{
		this->_rbm->hbias[h_counter] += rbm->hbias[h_counter];
	}


	//weight
	for (unsigned int v_counter = 0; v_counter < rbm->getAvailableSize(); v_counter++)
	{
		unsigned int vno = rbm->getAvailableTable()[v_counter];

		for (unsigned int h_counter = 0; h_counter < rbm->hlength; h_counter++)
		{
			this->_rbm->weight[vno][h_counter] += rbm->weight[vno][h_counter];
		}
	}

	//variance
	for (unsigned int v_counter = 0; v_counter < rbm->getAvailableSize(); v_counter++)
	{
		unsigned int vno = rbm->getAvailableTable()[v_counter];

		this->_rbm->variance[vno] += rbm->variance[vno];
	}

	//内部カウンタ
	this->_rbmcount++;

	for (unsigned int v_counter = 0; v_counter < rbm->getAvailableSize(); v_counter++)
	{
		unsigned int vno = rbm->getAvailableTable()[v_counter];
		this->_vsum[vno]++;
	}


	return true;
}

GBRBMCF *GBRBMAVG::getAvgRBMCF()
{
	GBRBMCF *avgrbm = GBRBMCFBuilder::duplicate(this->_rbm);

	if (this->_rbmcount < 1) return NULL;

	//vbias
	for (unsigned int v_counter = 0; v_counter < avgrbm->vlength; v_counter++)
	{
		avgrbm->vbias[v_counter] /= this->_vsum[v_counter] < 1 ? 1 : this->_vsum[v_counter];
	}

	//hbias
	for (unsigned int h_counter = 0; h_counter < avgrbm->hlength; h_counter++)
	{
		avgrbm->hbias[h_counter] /= this->_rbmcount;
	}


	//weight
	for (unsigned int v_counter = 0; v_counter < avgrbm->vlength; v_counter++)
	{
		for (unsigned int h_counter = 0; h_counter < avgrbm->hlength; h_counter++)
		{
			avgrbm->weight[v_counter][h_counter] /= this->_vsum[v_counter] < 1 ? 1 : this->_vsum[v_counter];
		}
	}

	//variance
	for (unsigned int v_counter = 0; v_counter < avgrbm->vlength; v_counter++)
	{
		avgrbm->variance[v_counter] /= this->_vsum[v_counter] < 1 ? 1 : this->_vsum[v_counter];
	}


	return avgrbm;
}

bool GBRBMAVG::write(char *fname)
{
	GBRBMCF *outrbm = this->getAvgRBMCF();

	GBRBMCFBuilder::write(*outrbm, fname);

	delete outrbm;

	return true;
}
