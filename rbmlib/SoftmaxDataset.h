/**
*	@file SoftmaxRBMDataset.h
*	@brief SoftmaxRBM用観測データ
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>

class SoftmaxDataset
{
public:
	unsigned int length; /** データのサイズ */
	unsigned int klength; /** 次元数 */
	std::vector<std::vector<double> > item; /** データ */;

	/**
		@brief コンストラクタ
		@param サイズ
		@param 次元数
	*/
	SoftmaxDataset(unsigned int length, unsigned int klength);

	/**
		@brief デストラクタ
	*/
	~SoftmaxDataset();

	/**
		@brief 値の取得
		@param index インデックス番号
		@return 値
	*/
	int getValue(unsigned int index);

	/**
		@brief 値のセット
		@param index インデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setValue(unsigned int index, int value);

	/**
		@brief vectorコンテナからデータをセット
		@param データ配列
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setData(std::vector<std::vector<double> > & item);

};

