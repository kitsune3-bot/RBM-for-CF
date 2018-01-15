/**
*	@file SoftmaxRBMTrain.h
*	@brief SoftmaxRBMの訓練クラス
*/
#pragma once
#include "SoftmaxRBM.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxDataset.h"

/**
	@class GBRBMTrain
	@brief GBRBMを学習させるクラス
*/
class SoftmaxRBMTrain
{
public:
	/**
		@brief コンストラクタ
		@param vlen 可視変数の数
		@param klen 可視変数の次元数
		@param hlen 隠れ変数の数
		@param epoch 学習の反復回数
		@param learning_rate 学習率
	*/
	SoftmaxRBMTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate);

	/**
		@brief コンストラクタ
		@param vlen 可視変数の数
		@param klen 可視変数の次元数
		@param hlen 隠れ変数の数
		@param epoch 学習の反復回数
		@param learning_rate 学習率
		@param cdk Contrastive Divergenceの反復回数
	*/
	SoftmaxRBMTrain(unsigned int vlen, unsigned int klen, unsigned int hlen, unsigned int epoch, double leaning_rate, unsigned int cdk);

	/**
		@brief デストラクタ
	*/
	~SoftmaxRBMTrain();

	/**
		@brief 各データごとに学習(データを捨てていないのでオンライン学習かどうかは微妙)
		@param rbm rbmオブジェクト
		@param 観測データセット
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool train(SoftmaxRBM & rbm, SoftmaxDatasetContainer & datasets);

	/**
		@brief 全データで学習(バッチ学習, 全データに対する勾配を計算)
		@param rbm rbmオブジェクト
		@param 観測データセット
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool trainBatch(SoftmaxRBM & rbm, SoftmaxDatasetContainer & datasets);

private:
	unsigned int epoch; /** 学習の反復回数 */
	double learningRate; /** 学習率 */
	unsigned int vlength; /** 可視変数の数 */
	unsigned int hlength; /**  隠れ変数の数 */
	unsigned int cdk = 1; /** Contrastive Divergenceの反復回数 */	unsigned int klength;
	std::vector< std::vector<double> > _tmpGradientVbias; /** 最後に計算した可視変数パラメータの勾配 */
	std::vector<double> _tmpGradientHbias; /** 最後に計算した隠れ変数パラメータの勾配 */
	std::vector< std::vector< std::vector<double> > > _tmpGradientWeight; /** 最後に計算した重みパラメータの勾配 */

	/**
		@brief データ1つからの勾配を計算 内部処理なのでプライベート関数へ移行予定
		@param rbm rbmオブジェクト
		@param dataset 観測データ
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool _getGradient(SoftmaxRBM & rbm, SoftmaxDataset & dataset);

	/**
		@brief Contrastive Divergence 内部処理なのでプライベート関数へ移行予定
		@param rbm rbmオブジェクト
		@param cdk Contrastive Divergenceの反復回数
		@return 成功したらtrue, 失敗でfalse.
	*/
	bool _contrastiveDivergence(SoftmaxRBM *rbm, unsigned int cdk);
};

