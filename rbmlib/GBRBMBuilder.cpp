#include "GBRBMBuilder.h"

GBRBMBuilder::GBRBMBuilder()
{
}


GBRBMBuilder::~GBRBMBuilder()
{
}

GBRBM *GBRBMBuilder::build(unsigned int vlength, unsigned int hlength)
{
	GBRBM *gbrbm = new GBRBM(vlength, hlength);

	return gbrbm;
}

GBRBM *GBRBMBuilder::build(unsigned int vlength, unsigned int hlength, GBRBMDataset & dataset)
{
	GBRBM *gbrbm = new GBRBM(vlength, hlength);
	GBRBMBuilder::setDataset(*gbrbm, dataset);

	return gbrbm;
}

GBRBM *GBRBMBuilder::build(char *fname)
{
	std::ifstream ifs(fname, std::ios::binary);

	unsigned int vlength;
	unsigned int hlength;
	
	ifs.read((char*)&vlength, sizeof(unsigned int));
	ifs.read((char*)&hlength, sizeof(unsigned int));

	GBRBM *gbrbm = GBRBMBuilder::build(vlength, hlength);

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

	ifs.close();

	return gbrbm;
}

GBRBM *GBRBMBuilder::duplicate(GBRBM & orig_rbm)
{
	GBRBM *gbrbm = new GBRBM(orig_rbm.vlength, orig_rbm.hlength);

	for (unsigned int v_counter = 0; v_counter < gbrbm->vlength; v_counter++)
	{
		gbrbm->vnodes[v_counter] =orig_rbm.vnodes[v_counter];
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

	return gbrbm;
}

GBRBM *GBRBMBuilder::duplicate(GBRBM *orig_rbm)
{
	GBRBM *gbrbm = GBRBMBuilder::duplicate(*orig_rbm);

	return gbrbm;
}

bool GBRBMBuilder::setDataset(GBRBM & rbm, GBRBMDataset & dataset)
{
	for (unsigned int v_counter = 0; v_counter < rbm.vlength; v_counter++)
	{
		rbm.vnodes[v_counter] = dataset.item[v_counter];
	}

	return true;
}

bool GBRBMBuilder::write(GBRBM & rbm, char *fname)
{

	std::ofstream ofs(fname, std::ios::out | std::ios::binary);

	if (!ofs) return false;

	ofs.write((const char *)&(rbm.vlength), sizeof(int));
	ofs.write((const char *)&(rbm.hlength), sizeof(int));


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


	return true;
}