#include "GBRBMCFBuilder.h"

GBRBMCFBuilder::GBRBMCFBuilder()
{
}


GBRBMCFBuilder::~GBRBMCFBuilder()
{
}

GBRBMCF *GBRBMCFBuilder::build(unsigned int vlength, unsigned int hlength)
{
	GBRBMCF *gbrbm = new GBRBMCF(vlength, hlength);

	return gbrbm;
}

GBRBMCF *GBRBMCFBuilder::build(unsigned int vlength, unsigned int hlength, GBRBMDataset & dataset)
{
	GBRBMCF *gbrbm = new GBRBMCF(vlength, hlength);
	GBRBMCFBuilder::setDataset(*gbrbm, dataset);

	return gbrbm;
}

GBRBMCF *GBRBMCFBuilder::build(char *fname)
{
	std::ifstream ifs(fname, std::ios::binary);

	unsigned int vlength;
	unsigned int hlength;

	ifs.read((char*)&vlength, sizeof(unsigned int));
	ifs.read((char*)&hlength, sizeof(unsigned int));

	GBRBMCF *gbrbm = GBRBMCFBuilder::build(vlength, hlength);

	//load vnode
	ifs.read((char*)&(gbrbm->vnodes.front()), sizeof(GBRBM::VNODETYPE) * gbrbm->vlength);

	//load hnode
	ifs.read((char*)&(gbrbm->hnodes.front()), sizeof(GBRBM::HNODETYPE) * gbrbm->hlength);

	//load vbias
	ifs.read((char*)&(gbrbm->vbias.front()), sizeof(GBRBM::VBIASTYPE) * gbrbm->vlength);

	//load hbias
	ifs.read((char*)&(gbrbm->hbias.front()), sizeof(GBRBM::HBIASTYPE) * gbrbm->hlength);

	//load weight
	for (unsigned int v_counter = 0; v_counter<gbrbm->vlength; v_counter++)
	{
		ifs.read((char*)&(gbrbm->weight[v_counter].front()), sizeof(GBRBM::VBIASTYPE) * gbrbm->hlength);
	}

	//load variance
	ifs.read((char*)&(gbrbm->variance.front()), sizeof(GBRBM::VARIANCETYPE) * gbrbm->vlength);

	/*
	* load tabledata
	*/
	unsigned int tsize;
	ifs.read((char*)&tsize, sizeof(unsigned int));
	gbrbm->disableVnodeAll();

	for (unsigned int i = 0; i < tsize; i++)
	{
		unsigned int t_value;
		ifs.read((char*)&t_value, sizeof(unsigned int));
		gbrbm->enableVnode(t_value);
	}

	ifs.close();

	return gbrbm;
}

GBRBMCF *GBRBMCFBuilder::duplicate(GBRBMCF & orig_rbm)
{
	GBRBMCF *gbrbm = new GBRBMCF(orig_rbm.vlength, orig_rbm.hlength);

	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		gbrbm->vnodes[v_counter] = orig_rbm.vnodes[v_counter];
		gbrbm->vbias[v_counter] = orig_rbm.vbias[v_counter];
		gbrbm->variance[v_counter] = orig_rbm.variance[v_counter];
	}

	for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
	{
		gbrbm->hnodes[h_counter] = orig_rbm.hnodes[h_counter];
		gbrbm->hbias[h_counter] = orig_rbm.hbias[h_counter];
	}

	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		for (unsigned int h_counter = 0; h_counter < gbrbm->hlength; h_counter++)
		{
			gbrbm->weight[v_counter][h_counter] = orig_rbm.weight[v_counter][h_counter];
		}
	}

	gbrbm->disableVnodeAll();
	auto table = orig_rbm.getAvailableTable();
	for (unsigned int & node : table)
	{
		gbrbm->enableVnode(node);
	}

	return gbrbm;
}

GBRBMCF *GBRBMCFBuilder::duplicate(GBRBMCF *orig_rbm)
{
	GBRBMCF *gbrbm = GBRBMCFBuilder::duplicate(*orig_rbm);

	return gbrbm;
}

bool GBRBMCFBuilder::setDataset(GBRBMCF & rbm, GBRBMDataset & dataset)
{
	double diff = 0.000001; //誤差

	//共通処理だ追い出せ
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		rbm.vnodes[v_counter] = dataset.item[v_counter];
	}
	//共通処理ここまで

	//テーブル作成
	rbm.disableVnodeAll();

	for (unsigned int v_counter = 0; v_counter < dataset.length; v_counter++)
	{
		if (!(0 - diff <= dataset.item[v_counter] && dataset.item[v_counter] <= 0 + diff)) rbm.enableVnode(v_counter);
	}

	return true;
}

bool GBRBMCFBuilder::write(GBRBMCF & rbm, char *fname)
{
		std::ofstream ofs(fname, std::ios::out | std::ios::binary);

		if (!ofs) return false;

		ofs.write((const char *)&(rbm.vlength), sizeof(unsigned int));
		ofs.write((const char *)&(rbm.hlength), sizeof(unsigned int));


		/*
		* write nodes
		*/
		ofs.write((const char *)&rbm.vnodes.front(), sizeof(GBRBM::VNODETYPE) * rbm.vlength);

		ofs.write((const char *)&rbm.hnodes.front(), sizeof(GBRBM::HNODETYPE) * rbm.hlength);

		/*
		* write biases
		*/
		ofs.write((const char *)&rbm.vbias.front(), sizeof(GBRBM::VBIASTYPE) * rbm.vlength);

		ofs.write((const char *)&rbm.hbias.front(), sizeof(GBRBM::HBIASTYPE) * rbm.hlength);

		/*
		* write weights
		*/
		for (unsigned int v_counter = 0; v_counter<rbm.vlength; v_counter++)
		{
			ofs.write((const char*)&rbm.weight[v_counter].front(), sizeof(GBRBM::WEIGHTTYPE) * rbm.hlength);
		}

		/*
		* write variances
		*/
		ofs.write((const char *)&rbm.variance.front(), sizeof(GBRBM::VARIANCETYPE) * rbm.vlength);

		/*
		* table data
		*/
		unsigned int tsize = (unsigned int)rbm.getAvailableSize();
		ofs.write((const char *)&(tsize), sizeof(int));
		auto table = rbm.getAvailableTable();

		for (unsigned int & node : table)
		{
			ofs.write((const char *)&(node), sizeof(unsigned int));
		}

		return true;
}