/**
*	@file GBRBM.h
*	@brief GBRBMクラスのヘッダーファイル
*/
#pragma once
#include "SoftmaxDataset.h"
#include "SoftmaxDatasetContainer.h"
#include <vector>

/**
	@class SoftmaxRBM
	@brief SoftmaxRBMを表すクラス
*/
class SoftmaxRBM
{
public:
	using VNODETYPE = double;
	using HNODETYPE = double;;
	using VBIASTYPE = double;;
	using HBIASTYPE = double;;
	using WEIGHTTYPE = double;;

	unsigned int          vlength; /** 可視変数の数 */
	unsigned int          hlength; /** 隠れ変数の数 */
	unsigned int          k; /** 次元数 */
	std::vector<std::vector<SoftmaxRBM::VNODETYPE> > vnodes; /** 可視変数 */
	std::vector<SoftmaxRBM::HNODETYPE> hnodes; /** 隠れ変数 */
	std::vector<std::vector<SoftmaxRBM::VBIASTYPE> > vbias; /** 可視変数のバイアスパラメータ */
	std::vector<SoftmaxRBM::HBIASTYPE> hbias; /** 隠れ変数のバイアスパラメータ */
	std::vector<std::vector<std::vector<SoftmaxRBM::WEIGHTTYPE> > > weight; /** リンクの重みパラメータ */

	/**
		@brief コンストラクタ
	*/
	SoftmaxRBM();

	/**
		@brief コンストラクタ
		@param vlength 可視変数の数
		@param hlength 隠れ変数の数
		@param k 次元数
	*/
	SoftmaxRBM(unsigned int vlength, unsigned int hlength, unsigned int k);

	/**
		@brief デストラクタ
	*/
	~SoftmaxRBM();
	
	/**
		@brief 可視変数の条件付き確率
		@param 可視変数インデックス番号
		@param 可視変数クラスインデックス番号
		@return 確率値
	*/
	double conditionalProbabilityVisible(unsigned int vindex, unsigned int kindex);

	/**
		@brief 隠れ変数の条件付き確率
		@param 隠れ変数インデックス番号
		@return 確率値
	*/
	double conditionalProbabilityHidden(unsigned int index);

	/**
		@brief 可視層, 隠れ層の順にブロックギブスサンプリング
		@param n 反復回数
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool gibbsSample(unsigned int n);

	/**
		@brief 可視層をブロックギブスサンプリング
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool gibbsSampleVisible();

	/**
		@brief 隠れ層をブロックギブスサンプリング
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool gibbsSampleHidden();

	/**
		@brief 可視変数の値を取得
		@param vindex 可視変数のインデックス番号
		@return 可視変数の値
	*/
	int getVnodeValue(unsigned int vindex);

	/**
		@brief 可視変数の値をセット
		@param vindex 可視変数のインデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setVnodeValue(unsigned int vindex, int value);

	/**
		@brief 可視変数の値を再構築
		@param vindex 可視変数のインデックス番号
		@return 再構築した可視変数の値
	*/
	unsigned int reconstructVnode(unsigned int vindex);

	/**
		@brief 平均場近似による可視変数の更新(未完成)
		@param vindex 可視変数のインデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool meanFieldUpdateVisible(unsigned int vindex);

	/**
		@brief 平均場近似による隠れ変数の更新(未完成)
		@param hindex 隠れ変数のインデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool meanFieldUpdateHidden(unsigned int hindex);

private:
	/**
	@brief Softmax RBM複製.
	@return 複製したSoftmax RBMオブジェクトのポインタ
	*/
	SoftmaxRBM *_clone();

	/**
	@brief Softmax RBMのコピー.
	@param targetRBM 複製先
	@return 成功したらtrue, 失敗でfalse.
	*/
	bool _copy(SoftmaxRBM *targetRBM);

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
	@brief 可視変数クラスK関係の結合項の総和 \sum_{j \in H}w_{ij}h_{j}
	@param vindex 可視変数インデックス番号
	@param vindex クラスインデックス番号
	@return 値
	*/
	double _getSumWeightHidden(unsigned int vindex, unsigned int kindex);


	/**
	@brief 隠れ変数関係の結合項の総和 \sum_{i \in V}\sum_{k=1}^{K}w_{ij}^{k}v_{i}^{k}
	@param hindex 隠れ変数インデックス番号
	@return 値
	*/
	double _getSumWeightVisible(unsigned int hindex);

	/**
	@brief ロジスティックシグモイド関数
	@param x 引数
	@return 値
	*/
	double _sigmoid(double x);
};

