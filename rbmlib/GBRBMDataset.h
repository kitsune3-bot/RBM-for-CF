/**
*	@file GBRBMDataset.h
*	@brief GBRBM用観測データ
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <vector>

class GBRBMDataset
{
public:
	unsigned int length; /** データのサイズ */
	std::vector<double> item; /** データ */

	/**
		@brief コンストラクタ
		@param サイズ
	*/
	GBRBMDataset(unsigned int length);

	/**
		@param デストラクタ
	*/
	~GBRBMDataset();

	/**
		@brief 値の取得
		@param index インデックス番号
		@return 値
	*/
	double getValue(unsigned int index);

	/**
		@brief 値のセット
		@param index インデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setValue(unsigned int index, double value);
	
	/**
		@brief vectorコンテナからデータをセット
		@param データ配列
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setData(std::vector<double> & item);
};

