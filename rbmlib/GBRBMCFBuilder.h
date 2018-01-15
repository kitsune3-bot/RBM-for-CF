#pragma once
#include "GBRBMBuilder.h"
#include "GBRBMCF.h"

class GBRBMCFBuilder :
	public GBRBMBuilder
{
public:
	GBRBMCFBuilder();
	~GBRBMCFBuilder();

	static GBRBMCF *build(unsigned int vlength, unsigned int hlength);

	static GBRBMCF *build(unsigned int vlength, unsigned int hlength, GBRBMDataset & dataset);

	/**
	@brief GBRBMをファイルからロード
	@param fname ファイル名
	@return GBRBMポインタ
	*/
	static GBRBMCF *build(char *fname);

	static GBRBMCF *duplicate(GBRBMCF & orig_rbm);

	static GBRBMCF *duplicate(GBRBMCF *orig_rbm);

	static bool setDataset(GBRBMCF & rbm, GBRBMDataset & dataset);

	/**
	@brief GBRBMをファイルに保存
	@param fname ファイル名
	@return 成功したらtrue, 失敗でfalse.
	*/
	static bool write(GBRBMCF & rbm, char *fname);
};

