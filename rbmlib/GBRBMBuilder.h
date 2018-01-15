/**
*	@file GBRBMBuilder.h
*	@brief GBRBMBuilderクラスのヘッダーファイル
*/
#pragma once
#include "GBRBM.h"
#include "GBRBMDatasetContainer.h"
#include "GBRBMDataset.h"
#include <fstream>

/**
*	@class GBRBMBuilder
*	@brief GBRBM構築クラス
*/
class GBRBMBuilder
{
public:
	/**
		@brief 
	*/
	GBRBMBuilder();
	~GBRBMBuilder();

	static GBRBM *build(unsigned int vlength, unsigned int hlength);

	static GBRBM *build(unsigned int vlength, unsigned int hlength, GBRBMDataset & dataset);

	/**
		@brief GBRBMをファイルからロード
		@param fname ファイル名
		@return GBRBMポインタ
	*/
	static GBRBM *build(char *fname);

	static GBRBM *duplicate(GBRBM & orig_rbm);

	static GBRBM *duplicate(GBRBM *orig_rbm);

	static bool setDataset(GBRBM & rbm, GBRBMDataset & dataset);

	/**
	@brief GBRBMをファイルに保存
	@param fname ファイル名
	@return 成功したらtrue, 失敗でfalse.
	*/
	static bool write(GBRBM & rbm, char *fname);

};

