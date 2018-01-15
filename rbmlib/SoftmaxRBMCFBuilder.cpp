#include "SoftmaxRBMCFBuilder.h"



SoftmaxRBMCFBuilder::SoftmaxRBMCFBuilder()
{
}


SoftmaxRBMCFBuilder::~SoftmaxRBMCFBuilder()
{
}

SoftmaxRBMCF *SoftmaxRBMCFBuilder::build(unsigned int vlength, unsigned int hlength, unsigned int k)
{
	SoftmaxRBMCF *gbrbm = new SoftmaxRBMCF(vlength, hlength, k);

	return gbrbm;
}

SoftmaxRBMCF *SoftmaxRBMCFBuilder::build(unsigned int vlength, unsigned int hlength, unsigned int k, SoftmaxDataset & dataset)
{
	SoftmaxRBMCF *gbrbm = new SoftmaxRBMCF(vlength, hlength, k);
	SoftmaxRBMBuilder::setDataset(*gbrbm, dataset);

	return gbrbm;
}

SoftmaxRBMCF *SoftmaxRBMCFBuilder::build(char *fname)
{
	std::ifstream ifs(fname, std::ios::binary);

	unsigned int vlength;
	unsigned int hlength;
	unsigned int k;

	ifs.read((char*)&vlength, sizeof(int));
	ifs.read((char*)&hlength, sizeof(int));
	ifs.read((char*)&k, sizeof(int));

	SoftmaxRBMCF *srbm = SoftmaxRBMCFBuilder::build(vlength, hlength, k);

	//load vnode
	for (unsigned int v_counter = 0; v_counter<srbm->vlength; v_counter++)
	{
		ifs.read((char*)&(srbm->vnodes[v_counter].front()), sizeof(SoftmaxRBM::VNODETYPE) * srbm->k);
	}

	//load hnode
	ifs.read((char*)&(srbm->hnodes.front()), sizeof(SoftmaxRBM::HNODETYPE) * srbm->hlength);


	//load vbias
	for (unsigned int v_counter = 0; v_counter<srbm->vlength; v_counter++)
	{
		ifs.read((char*)&(srbm->vbias[v_counter].front()), sizeof(SoftmaxRBM::VBIASTYPE) * srbm->k);
	}

	//load hbias
	ifs.read((char*)&(srbm->hbias.front()), sizeof(SoftmaxRBM::HBIASTYPE) * srbm->hlength);

	//load weight
	for (unsigned int v_counter = 0; v_counter<srbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter<srbm->k; k_counter++)
		{
			ifs.read((char*)&(srbm->weight[v_counter][k_counter].front()), sizeof(SoftmaxRBM::VBIASTYPE) * srbm->hlength);
		}
	}

	ifs.close();

	return srbm;
}

SoftmaxRBMCF *SoftmaxRBMCFBuilder::duplicate(SoftmaxRBMCF & orig_rbm)
{
	SoftmaxRBMCF *srbm = SoftmaxRBMCFBuilder::build(orig_rbm.vlength, orig_rbm.hlength, orig_rbm.k);

	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			srbm->vnodes[v_counter][k_counter] = orig_rbm.vnodes[v_counter][k_counter];
			srbm->vbias[v_counter][k_counter] = orig_rbm.vbias[v_counter][k_counter];
		}
	}

	for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
	{
		srbm->hnodes[h_counter] = orig_rbm.hnodes[h_counter];
	}

	for (unsigned int v_counter = 0; v_counter < srbm->vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < srbm->k; k_counter++)
		{
			for (unsigned int h_counter = 0; h_counter < srbm->hlength; h_counter++)
			{
				srbm->weight[v_counter][k_counter][h_counter] = orig_rbm.weight[v_counter][k_counter][h_counter];
			}
		}
	}

	srbm->disableVnodeAll();
	auto table = orig_rbm.getAvailableTable();
	for (unsigned int & node : table)
	{
		srbm->enableVnode(node);
	}

	return srbm;
}

SoftmaxRBMCF *SoftmaxRBMCFBuilder::duplicate(SoftmaxRBMCF *orig_rbm)
{
	SoftmaxRBMCF *srbm = SoftmaxRBMCFBuilder::duplicate(*orig_rbm);
	return srbm;
}

bool SoftmaxRBMCFBuilder::setDataset(SoftmaxRBMCF & rbm, SoftmaxDataset & dataset)
{
	double diff = 0.0001;

	//共通処理だ追い出せ
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter < rbm.k; k_counter++)
		{
			rbm.vnodes[v_counter][k_counter] = dataset.item[v_counter][k_counter];
		}
	}
	//共通処理ここまで

	//テーブル作成
	//サイズあってる？
	rbm.disableVnodeAll();

	for (unsigned int v_counter = 0; v_counter < dataset.length; v_counter++)
	{
		unsigned int sum = 0;

		for (unsigned int k_counter = 0; k_counter < dataset.klength; k_counter++)
		{
			//0以外
			sum += !(fabs(dataset.item[v_counter][k_counter]) < diff) ? 1 : 0;
		}

		//追加処理あってる?
		if (sum == 1) rbm.enableVnode(v_counter);
	}

	return true;
}

bool SoftmaxRBMCFBuilder::write(SoftmaxRBMCF & rbm, char *fname)
{
	std::ofstream ofs(fname, std::ios::out | std::ios::binary);

	if (!ofs) return false;

	ofs.write((const char *)&(rbm.vlength), sizeof(int));
	ofs.write((const char *)&(rbm.hlength), sizeof(int));
	ofs.write((const char *)&(rbm.k), sizeof(int));


	/*
	* write nodes
	*/
	for (unsigned int v_counter = 0; v_counter<rbm.vlength; v_counter++)
	{
		ofs.write((const char *)&rbm.vnodes[v_counter].front(), sizeof(SoftmaxRBM::VNODETYPE) * rbm.k);
	}

	ofs.write((const char *)&rbm.hnodes.front(), sizeof(SoftmaxRBM::HNODETYPE) * rbm.hlength);

	/*
	* write biases
	*/
	for (unsigned int v_counter = 0; v_counter<rbm.vlength; v_counter++)
	{
		ofs.write((const char *)&rbm.vbias[v_counter].front(), sizeof(SoftmaxRBM::VBIASTYPE) * rbm.k);
	}

	ofs.write((const char *)&rbm.hbias.front(), sizeof(SoftmaxRBM::HBIASTYPE) * rbm.hlength);

	/*
	* write weights
	*/
	for (unsigned int v_counter = 0; v_counter<rbm.vlength; v_counter++)
	{
		for (unsigned int k_counter = 0; k_counter<rbm.k; k_counter++)
		{
			ofs.write((const char*)&rbm.weight[v_counter][k_counter].front(), sizeof(SoftmaxRBM::WEIGHTTYPE) * rbm.hlength);
		}
	}


	return true;
}