#include "SoftmaxDataset.h"
#include <vector>

SoftmaxDataset::SoftmaxDataset(unsigned int length, unsigned int klength)
{
	this->length = length;
	this->klength = klength;
	this->item.assign(length, std::vector<double>(klength));
}

SoftmaxDataset::~SoftmaxDataset()
{

}

int SoftmaxDataset::getValue(unsigned int index)
{
	int sum = 0;
	int value = 0;
	unsigned int k_counter;
	double diff = 0.00001;

	//error check
	if (this->length < index) return 0;

	//calc
	for (k_counter = 0; k_counter<this->klength; k_counter++)
	{
		if ( fabs(this->item[index][k_counter] - 1.0 ) < diff) {
			sum += 1;
			value = k_counter + 1;
		}
	}

	//error check
	if (1 < sum) return 0;

	return value;
}

bool SoftmaxDataset::setValue(unsigned int index, int value)
{
	//error check
	if (this->length < index) return false;

	//set value
	for (unsigned int k_counter = 0; k_counter < this->klength; k_counter++)
	{
		this->item[index][k_counter] = (k_counter == (value - 1) ) ? 1 : 0;
	}

	return true;
}

bool SoftmaxDataset::setData(std::vector<std::vector<double> > & item)
{
	//error check
	if (item.size() != this->length) return false;

	this->item = item;

	return true;
}