#pragma once
#include "GBRBMCF.h"
#include "GBRBMCFBuilder.h"
#include <vector>

class GBRBMAVG
{
private:
	unsigned int _rbmcount = 0;
	GBRBMCF *_rbm;
	std::vector<unsigned int> _vsum;

public:
	GBRBMAVG(unsigned int vlength, unsigned int hlength);
	~GBRBMAVG();
	bool addParameters(GBRBMCF *rbm);
	GBRBMCF *getAvgRBMCF();
	bool write(char *fname);
};

