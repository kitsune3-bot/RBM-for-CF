/**
*	@file SoftmaxRBMCF.h
*	@brief SoftmaxRBMCFクラスのヘッダーファイル
*/
#pragma once
#include "SoftmaxRBM.h"
#include <deque>

/**
	@class SoftmaxRBMCF
	@brief 協調フィルタリング用のSoftmaxRBMクラス
*/
class SoftmaxRBMCF :
	public SoftmaxRBM
{
public:
	/**
		@brief コンストラクタ
	*/
	SoftmaxRBMCF();

	/**
		@brief コンストラクタ
		@param vlength 可視変数の数
		@param hlength 隠れ変数の数
		@param k 次元数
	*/
	SoftmaxRBMCF(unsigned int vlength, unsigned int hlength, unsigned int k);

	/**
		@brief デストラクタ
	*/
	~SoftmaxRBMCF();
	
	//additional method
	/**
		@brief 既知データのみからなるRBMの取得
		@return　SoftmaxRBMオブジェクトのポインタ
	*/
	SoftmaxRBM *getAvailableRBM();

	/**
		@brief既知データのみからなるRBMをSoftmaxRBMCFに戻す
		@param gbrbm SoftmaxRBMオブジェクトのポインタ
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setAvailableRBM(SoftmaxRBM *srbm);

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
	unsigned int predict(unsigned int vnodeno);

	/**
	@brief 可視変数を未知データにする
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
	@brief GBRBM初期化.
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _init();


	/**
	@brief SoftmaxRBMCF複製.
	@return 成功したらtrue, 失敗でfalse.
	*/
	SoftmaxRBMCF *_clone();

	/**
	@brief SoftmaxRBMCFコピー.
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _copy(SoftmaxRBMCF *targetRBM);
};

