/**
*	@file GBRBMDatasetContainer.h
*	@brief GBRBM用観測データセット GBRBMDatasetを格納
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "GBRBMDataset.h"
#include <vector>

class GBRBMDatasetContainer
{
public:
	/**
		@brief コンストラクタ
		@param file_name CSVファイル名
		@param container_size 観測データ数
		@param container_size 観測データサイズ
	*/
	GBRBMDatasetContainer(char *file_name, unsigned int container_size, unsigned int data_size);

	/**
		@brief コンストラクタ
		@param container_size 観測データ数
		@param container_size 観測データサイズ
	*/
	GBRBMDatasetContainer(unsigned int container_size, unsigned int data_size);

	/**
		@brief デストラクタ
	*/
	~GBRBMDatasetContainer();
	
	GBRBMDataset & operator[](unsigned int i);
	
	/**
		@brief CSVファイルのロード
		@param file_name CSVファイル名
		@param container_size 観測データ数
		@param data_size 観測データサイズ
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool load(char *file_name, unsigned int container_size, unsigned int data_size);

	/**
		@brief 値のセット メソッド名が不適切なので変更予定
		@param dataset 観測データ
		@param indec インデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setDataset(GBRBMDataset dataset, unsigned int index);


	/**
		@brief データセットの総数を取得
		@return データセットの総数
	*/
	unsigned int getLength();

private:
	std::vector<GBRBMDataset> _item;
};