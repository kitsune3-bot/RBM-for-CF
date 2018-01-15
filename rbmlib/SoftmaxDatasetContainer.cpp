#include "SoftmaxDatasetContainer.h"
#include <vector>
#include <cstdio>
#include <cstring>



SoftmaxDatasetContainer::SoftmaxDatasetContainer(char *file_name, unsigned int container_size, unsigned int data_size, unsigned int k_size)
{
	this->_item.assign(container_size, SoftmaxDataset(data_size, k_size));
	this->load(file_name, container_size, data_size, k_size);
}

SoftmaxDatasetContainer::SoftmaxDatasetContainer(unsigned int container_size, unsigned int data_size, unsigned int k_size)
{
	this->_item.assign(container_size, SoftmaxDataset(data_size, k_size));
}


SoftmaxDatasetContainer::~SoftmaxDatasetContainer()
{
}

SoftmaxDataset & SoftmaxDatasetContainer::operator[](unsigned int i)
{
	return this->_item[i];
}

bool SoftmaxDatasetContainer::load(char *file_name, unsigned int container_size, unsigned int data_size, unsigned int k_size)
{
	FILE *fp;

	//if (this->_item.size() < container_size) return false;
	this->_item.assign(container_size, SoftmaxDataset(data_size, k_size));


	if ((fp = std::fopen(file_name, "r")) == NULL) return false;

	for (unsigned c_counter = 0; c_counter < container_size; c_counter++)
	{
		char line_buffer[0xff];
		char *dat_ptr;
		std::fgets(line_buffer, 0xff, fp);
		dat_ptr = std::strtok(line_buffer, ",");
		dat_ptr = strtok(NULL, ",");//ユーザーID消去
		//skip name(comment)
		//sscanf(dat_ptr, "%d", &(softmaxdataset->label));

		for (unsigned data_counter = 0; data_counter < data_size; data_counter++)
		{
			int value;
			//データは整数値で書かれてる
			sscanf(dat_ptr, "%d", &value);
			for (unsigned int k_counter = 0; k_counter < k_size; k_counter++)
			{
				this->operator[](c_counter).item[data_counter][k_counter] = (k_counter + 1 == value) ? 1 : 0;
			}
			dat_ptr = strtok(NULL, ",");
		}
	}

	std::fclose(fp);

	return true;
}

bool SoftmaxDatasetContainer::setDataset(SoftmaxDataset dataset, unsigned int index)
{
	this->_item[index] = dataset;

	return true;
}

unsigned int SoftmaxDatasetContainer::getLength()
{
	return this->_item.size();
}