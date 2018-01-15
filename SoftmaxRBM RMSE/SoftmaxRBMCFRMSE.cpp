#include "SoftmaxRBMCFRMSE.h"
#include <cmath>
//#include <iostream>


SoftmaxRBMCFRMSE::SoftmaxRBMCFRMSE(SoftmaxRBMCF rbm, SoftmaxDataset dataset)
{
	SoftmaxRBMCFBuilder::setDataset(rbm, dataset);
	this->errors.assign(rbm.getAvailableSize(), 0);

	SoftmaxRBM *rbm_orig = rbm.getAvailableRBM();
	SoftmaxRBM *rbm_result = SoftmaxRBMBuilder::duplicate(rbm_orig);

	//rbm_result->gibbsSampleHidden();
	//rbm_result->gibbsSampleVisible();

	this->rmse = 0;
	for (unsigned int v_counter = 0; v_counter < rbm_orig->vlength; v_counter++)
	{
		unsigned int value_o = rbm_orig->getVnodeValue(v_counter);
		unsigned int value_r = rbm_result->reconstructVnode(v_counter);
		this->errors[v_counter] = std::sqrt(std::pow((double)value_o - (double)value_r, 2));

		this->rmse += std::pow((double)value_o - (double)value_r, 2);
		//std::cout << "<" << value_r << "/" << value_o << ">:" << std::abs((double)value_o - (double)value_r) << std::endl;
	}
	this->rmse /= rbm_orig->vlength;
	this->rmse = std::sqrt(this->rmse);

	delete rbm_orig;
	delete rbm_result;

}


SoftmaxRBMCFRMSE::~SoftmaxRBMCFRMSE()
{
}
