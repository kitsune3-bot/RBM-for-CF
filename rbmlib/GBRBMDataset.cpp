#include "GBRBMDataset.h"
#include <vector>


GBRBMDataset::GBRBMDataset(unsigned int length)
{
	this->length = length;
	this->item.assign(length, 0);
}


GBRBMDataset::~GBRBMDataset()
{
}

double GBRBMDataset::getValue(unsigned int index)
{
	//error check
	if (index < 0 || this->length < index) return 0;

	return this->item[index];
}

bool GBRBMDataset::setValue(unsigned int index, double value)
{
	//error check
	if (index < 0 || this->length < index) return 0;

	//set value
	this->item[index] = value;

	return true;
}

bool GBRBMDataset::setData(std::vector<double> & item)
{
	//error check
	if (item.size() != this->length) return false;

	this->item = item;

	return true;
}