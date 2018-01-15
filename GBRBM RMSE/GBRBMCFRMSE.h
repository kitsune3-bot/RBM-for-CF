#pragma once
#include "GBRBMCF.h"
#include "GBRBMDataset.h"
#include "GBRBMCFBuilder.h"
#include <vector>


class GBRBMCFRMSE
{
public:
	std::vector<double> errors;
	double rmse;

	GBRBMCFRMSE(GBRBMCF & rbm, GBRBMDataset & dataset);
	~GBRBMCFRMSE();
};

