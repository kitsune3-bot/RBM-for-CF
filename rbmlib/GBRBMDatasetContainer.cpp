#include "GBRBMDatasetContainer.h"



GBRBMDatasetContainer::GBRBMDatasetContainer(char *file_name, unsigned int container_size, unsigned int data_size)
{
	this->_item.assign(container_size, GBRBMDataset(data_size));
	this->load(file_name, container_size, data_size);
}

GBRBMDatasetContainer::GBRBMDatasetContainer(unsigned int container_size, unsigned int data_size)
{
	this->_item.assign(container_size, GBRBMDataset(data_size));
}

GBRBMDatasetContainer::~GBRBMDatasetContainer()
{
}

GBRBMDataset & GBRBMDatasetContainer::operator[](unsigned int i)
{
	return this->_item[i];
}

bool GBRBMDatasetContainer::load(char *file_name, unsigned int container_size, unsigned int data_size)
{
	FILE *fp;

	//if (this->_item.size() < container_size) return false;
	this->_item.assign(container_size, GBRBMDataset(data_size));


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
			sscanf(dat_ptr, "%d", &value);
			this->operator[](c_counter).item[data_counter] = (double) value;

			dat_ptr = strtok(NULL, ",");
		}
	}

	std::fclose(fp);

	return true;
}

bool GBRBMDatasetContainer::setDataset(GBRBMDataset dataset, unsigned int index)
{
	this->_item[index] = dataset;

	return true;
}

unsigned int GBRBMDatasetContainer::getLength()
{
	return this->_item.size();
}

