#include "SoftmaxRBMAvg.h"



SoftmaxRBMAvg::SoftmaxRBMAvg(unsigned int vlength, unsigned int hlength, unsigned int k)
{
	this->_rbm = new SoftmaxRBMCF(vlength, hlength, k);
	this->_vsum.assign(this->_rbm->vlength, 0);

	//vbias
	for (unsigned int v_counter = 0; v_counter < this->_rbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < this->_rbm->k; k_counter++)
		{
			this->_rbm->vbias[v_counter][k_counter] = 0;
		}
	}

	//hbias
	for (unsigned int h_counter = 0; h_counter < this->_rbm->hlength; h_counter++)
	{
		this->_rbm->hbias[h_counter] = 0;
	}


	//weight
	for (unsigned int v_counter = 0; v_counter < this->_rbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < this->_rbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < this->_rbm->hlength; h_counter++)
			{
				this->_rbm->weight[v_counter][k_counter][h_counter] = 0;
			}
		}
	}
}


SoftmaxRBMAvg::~SoftmaxRBMAvg()
{
	delete this->_rbm;
}

bool SoftmaxRBMAvg::addParameters(SoftmaxRBMCF *rbm)
{
	if (this->_rbm->vlength != rbm->vlength ||
		this->_rbm->k != rbm->k ||
		this->_rbm->hlength != rbm->hlength) {

		return false;
	}

	//vbias
	for (unsigned int v_counter = 0; v_counter < rbm->getAvailableSize(); v_counter++)
	{
		unsigned int vno = rbm->getAvailableTable()[v_counter];

		for (unsigned int k_counter = 0; k_counter < rbm->k; k_counter++)
		{
			this->_rbm->vbias[vno][k_counter] += rbm->vbias[vno][k_counter];
		}
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

		for (unsigned int k_counter = 0; k_counter < rbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < rbm->hlength; h_counter++)
			{
				this->_rbm->weight[vno][k_counter][h_counter] += rbm->weight[vno][k_counter][h_counter];
			}
		}
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

SoftmaxRBMCF *SoftmaxRBMAvg::getAvgRBMCF()
{
	SoftmaxRBMCF *avgrbm = SoftmaxRBMCFBuilder::duplicate(this->_rbm);

	if (this->_rbmcount < 1) return NULL;

	//vbias
	for (unsigned int v_counter = 0; v_counter < avgrbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < avgrbm->k; k_counter++)
		{
			avgrbm->vbias[v_counter][k_counter] /= this->_vsum[v_counter] < 1 ? 1 : this->_vsum[v_counter];
		}
	}

	//hbias
	for (unsigned int h_counter = 0; h_counter < avgrbm->hlength; h_counter++)
	{
		avgrbm->hbias[h_counter] /= this->_rbmcount;
	}


	//weight
	for (unsigned int v_counter = 0; v_counter < avgrbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < avgrbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < avgrbm->hlength; h_counter++)
			{
				avgrbm->weight[v_counter][k_counter][h_counter] /= this->_vsum[v_counter] < 1 ? 1 : this->_vsum[v_counter];
			}
		}
	}

	return avgrbm;
}

bool SoftmaxRBMAvg::write(char *fname)
{
	SoftmaxRBMCF *outrbm = this->getAvgRBMCF();

	SoftmaxRBMCFBuilder::write(*outrbm, fname);

	delete outrbm;

	return true;
}
