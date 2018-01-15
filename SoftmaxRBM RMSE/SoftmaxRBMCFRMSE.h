#pragma once
#include "SoftmaxRBMCF.h"
#include "SoftmaxDataset.h"
#include "SoftmaxRBMCFBuilder.h"
#include <vector>

class SoftmaxRBMCFRMSE
{
public:
	std::vector<double> errors;
	double rmse;

	SoftmaxRBMCFRMSE(SoftmaxRBMCF rbm, SoftmaxDataset dataset);
	~SoftmaxRBMCFRMSE();
};

