#pragma once
#include "SoftmaxRBMBuilder.h"
#include "SoftmaxRBMCF.h"
#include <fstream>

class SoftmaxRBMCFBuilder :
	public SoftmaxRBMBuilder
{
public:
	SoftmaxRBMCFBuilder();
	~SoftmaxRBMCFBuilder();
	static SoftmaxRBMCF *build(unsigned int vlength, unsigned int hlength, unsigned int k);

	static SoftmaxRBMCF *build(unsigned int vlength, unsigned int hlength, unsigned int k, SoftmaxDataset & dataset);

	/**
	@brief SoftmaxRBMをファイルからロード
	@param fname ファイル名
	@return SoftmaxRBMポインタ
	*/
	static SoftmaxRBMCF *build(char *fname);

	static SoftmaxRBMCF *duplicate(SoftmaxRBMCF & orig_rbm);

	static SoftmaxRBMCF *duplicate(SoftmaxRBMCF *orig_rbm);

	static bool setDataset(SoftmaxRBMCF & rbm, SoftmaxDataset & dataset);

	/**
	@brief SoftmaxRBMをファイルに保存
	@param fname ファイル名
	@return 成功したらtrue, 失敗でfalse.
	*/
	static bool write(SoftmaxRBMCF & rbm, char *fname);
};

