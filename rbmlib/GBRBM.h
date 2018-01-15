/**
*	@file GBRBM.h
*	@brief GBRBMクラスのヘッダーファイル
*/
#pragma once
#define _USE_MATH_DEFINES

#include "GBRBMDataset.h"
#include "GBRBMDatasetContainer.h"
#include <vector>

/**
  @class GBRBM
  @brief GBRBMを表すクラス
*/
class GBRBM
{
public:
	using VNODETYPE = double;
	using HNODETYPE = double;
	using VBIASTYPE = double;
	using HBIASTYPE = double;
	using WEIGHTTYPE = double;
	using VARIANCETYPE = double;

	unsigned int          vlength; /** 可視変数の数 */
	unsigned int          hlength; /** 隠れ変数の数 */
	std::vector<GBRBM::VNODETYPE> vnodes; /** 可視変数 */
	std::vector<GBRBM::HNODETYPE> hnodes; /** 隠れ変数 */
	std::vector<GBRBM::VBIASTYPE> vbias; /** 可視変数のバイアスパラメータ */
	std::vector<GBRBM::HBIASTYPE> hbias; /** 隠れ変数のバイアスパラメータ */
	std::vector<std::vector<GBRBM::WEIGHTTYPE> > weight; /** リンクの重みパラメータ */
	std::vector<GBRBM::VARIANCETYPE> variance; /** 可視変数の分散パラメータ */


	/**
		@brief 使用不可
	*/
	GBRBM();

	/**
		@brief コンストラクタ
		@param vlength 可視変数の数
		@param hlength 隠れ変数の数
	*/
	GBRBM(unsigned int vlength, unsigned int hlength);

	/**
		@brief デストラクタ
	*/
	~GBRBM();


	/**
		@brief 可視変数の条件付き確率
		@param 可視変数インデックス番号
		@return 確率値
	*/
	double conditionalProbabilityVisible(unsigned int vindex);

	/**
		@brief 隠れ変数の条件付き確率
		@param 隠れ変数インデックス番号
		@return 確率値
	*/
	double conditionalProbabilityHidden(unsigned int hindex);

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
	double getVnodeValue(unsigned int vindex);

	/**
		@brief 可視変数の値をセット
		@param vindex 可視変数のインデックス番号
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool setVnodeValue(unsigned int vindex, double value);

	/**
		@brief 可視変数の値を再構築
		@param vindex 可視変数のインデックス番号
		@return 再構築した可視変数の値
	*/
	double reconstructVnode(unsigned int vindex);

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
		@brief GBRBM複製.
		@return 複製したGBRBMオブジェクトのポインタ
		*/
		GBRBM *_clone();

		/**
		@brief GBRBMコピー.
		@param 複製先
		@return 成功したらtrue, 失敗でfalse.
		*/
		bool _copy(GBRBM *targetRBM);

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
		@brief可視変数関係の結合項の総和 \sum_{j \in H}w_{ij}h_{j}
		@param vindex 可視変数インデックス番号
		@return 値
		*/	double _getSumWeightHidden(unsigned int vindex);

		/**
		@brief 隠れ変数関係の結合項の総和 \sum_{i \in V}w_{ij}v_{i}
		@param hindex 隠れ変数インデックス番号
		@return 値
		*/
		double _getSumWeightVisible(unsigned int hindex);

		/**
		@brief ロジスティックシグモイド関数
		@return 確率値
		*/
		double _sigmoid(double x);

		/**
		@brief ガウス分布の確率密度関数
		@return 確率値
		*/
		double _gaussianDist(double x, double mu, double sigma);

		/**
		@brief b_i + {\sum_{j \in H} w_{ij}h_{j} }
		*/
		double _mu(unsigned int vindex);

		/**
		@brief c_j + {\sum_{i \in V} w_{ij}v_{i} }
		*/
		double _lambda(unsigned int hindex);
};
