#pragma once
#include "SoftmaxRBMCF.h"
#include "SoftmaxRBMCFBuilder.h"
#include <vector>

class SoftmaxRBMAvg
{
private:
	unsigned int _rbmcount = 0;
	SoftmaxRBMCF *_rbm;
	std::vector<unsigned int> _vsum;

public:
	SoftmaxRBMAvg(unsigned int vlength, unsigned int hlength, unsigned int k);
	~SoftmaxRBMAvg();
	bool addParameters(SoftmaxRBMCF *rbm);
	SoftmaxRBMCF *getAvgRBMCF();
	bool whitening();
	bool write(char *fname);
};

