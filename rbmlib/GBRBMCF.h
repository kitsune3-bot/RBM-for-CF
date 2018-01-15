/**
*	@file GBRBMCF.h
*	@brief GBRBMCFクラスのヘッダーファイル
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "GBRBM.h"
#include "GBRBMDataset.h"
#include "GBRBMDatasetContainer.h"
#include <deque>

/**
	@class GBRBMCF
	@brief 協調フィルタリング用のGBRBMクラス
*/
class GBRBMCF :
	public GBRBM
{

public:
	/**
		@brief コンストラクタ
	*/
	GBRBMCF();

	/**
		@brief コンストラクタ
		@param vlength 可視変数の数
		@param hlength 隠れ変数の数
	*/
	GBRBMCF(unsigned int vlength, unsigned int hlength);

	/**
		@brief デストラクタ
	*/
	~GBRBMCF();


	//additional method
	/**
		@brief 既知データのみからなるRBMの取得
		@return GBRBMオブジェクトのポインタ
	*/
	GBRBM *getAvailableRBM();

	/**
		@brief既知データのみからなるRBMをGBRBMCFに戻す
		@param gbrbm GBRBMオブジェクトのポインタ
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setAvailableRBM(GBRBM *gbrbm);

	/**
		@brief 既知データ一覧取得
		@return 既知データ一覧コンテナ
	*/
	std::deque<unsigned int> getAvailableTable();

	/**
		@brief 既知データ数を取得
		@return 既知データ数
	*/
	size_t getAvailableSize();

	/**
		@brief 値の推定
		@param vnodeno 推定するノード番号
		@return 推定値
	*/
	GBRBM::VNODETYPE predict(unsigned int vnodeno);

	/**
		@brief 該当可視変数を未知データにする
		@param vnodeno 可視変数のノード番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool disableVnode(unsigned int vnodeno);

	/**
	@brief 可視変数をすべて未知データにする
	@param vnodeno 可視変数のノード番号
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool disableVnodeAll();

	/**
		@brief 該当可視変数を既知データにする
		@param vnodeno 可視変数のノード番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool enableVnode(unsigned int vnodeno);

	/**
	@brief 可視変数をすべて既知データにする
	@param vnodeno 可視変数のノード番号
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool enableVnodeAll();

	/**
		@brief 平均場近似したGBRBMを取得(未完成)
		@return  更新後のGBRBM
	*/
	GBRBM *getMeanFieldGBRBM();

	/**
		@brief 平均場近似によるノードの更新(未完成)
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool meanFieldUpdate();

private:
	std::deque<unsigned int> _table = std::deque<unsigned int>(); /** 既知データ一覧 */

																  /**
																  @brief GBRBM作成.
																  @return 成功したらtrue, 失敗でfalse.
																  */
	bool _create();

	/**
	@brief 可視変数作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createVnodes();

	/**
	@brief 隠れ変数作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createHnodes();

	/**
	@brief 可視変数バイアスパラメータ作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createVbias();

	/**
	@brief 隠れ変数バイアスパラメータ作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createHbias();

	/**
	@brief 重みパラメータ作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createWeight();

	/**
	@brief 可視変数分散パラメータ作成
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _createVariance();

	/**
	@brief GBRBM初期化.
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _init();

	/**
	@brief GBRBMコピー.
	@return 成功したらtrue, 失敗でfalse.
	*/
	GBRBMCF *_clone();

	/**
	@brief GBRBMコピー.
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _copy(GBRBMCF *targetRBM);

};

