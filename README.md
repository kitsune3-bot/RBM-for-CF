# RBM-for-CF

制限ボルツマンマシンを使った協調フィルタリング.
(学部4年の卒研テーマ)

* SoftmaxRBMCF  
元の論文のモデル  
(入力が1-of-K表現)

* GBRBMCF  
Gaussian-Bernoulli RBMを使った協調フィルタリング。  
(入力を実数値に変更)  

元の論文  
http://www.machinelearning.org/proceedings/icml2007/papers/407.pdf

___

## ディレクトリ構成
* \* AVG  
実験結果の平均値を計算するプログラム
* \* RMSE  
推定結果のRMSEを計算するプログラム
* \* CF Trainer    
学習をする(バッチ学習)  
* \* CF Trainer Online  
学習をする(オンライン学習)  
* \* CF Predictor    
欠損値の推定を行う    
* rbmlib    
RBMのライブラリ各種    
** データセットの管理    
** 各種RBM    
** ビルダークラス    
