#pragma once
#include "SoftmaxRBM.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxDataset.h"
#include <fstream>

class SoftmaxRBMBuilder
{
public:
	SoftmaxRBMBuilder();
	~SoftmaxRBMBuilder();

	static SoftmaxRBM *build(unsigned int vlength, unsigned int hlength, unsigned int k);

	static SoftmaxRBM *build(unsigned int vlength, unsigned int hlength, unsigned int k,  SoftmaxDataset & dataset);

	static SoftmaxRBM *build(char *fname);

	static SoftmaxRBM *duplicate(SoftmaxRBM & orig_rbm);

	static SoftmaxRBM *duplicate(SoftmaxRBM *orig_rbm);

	static bool setDataset(SoftmaxRBM & rbm, SoftmaxDataset & dataset);

	/**
		@brief SoftmaxRBMをファイルに保存
		@param fname ファイル名
		@return 成功したらtrue, 失敗でfalse.
	*/
	static bool write(SoftmaxRBM & rbm, char *fname);

};

