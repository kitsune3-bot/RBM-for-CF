#include "SoftmaxDataset.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxRBM.h"
#include "SoftmaxRBMCF.h"
#include "SoftmaxRBMCFBuilder.h"
#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <cmath>

int main(int argc, char **argv)
{
	//オプション関連
	boost::program_options::variables_map vm;
	boost::program_options::options_description option("Option");
	option.add_options()
		("srbm", boost::program_options::value<std::string>(), "srbm file")
		("dataset", boost::program_options::value<std::string>(), "dataset csv file")
		("dsize", boost::program_options::value<unsigned int>(), "dsize")
		("vsize", boost::program_options::value<unsigned int>(), "vsize")
		("ksize", boost::program_options::value<unsigned int>(), "ksize");
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, option), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << option << std::endl;
		exit(-1);
	}

	const char *srbm = vm["srbm"].as<std::string>().data();
	const char *dataset = vm["dataset"].as<std::string>().data();
	unsigned int dsize = vm["dsize"].as<unsigned int>();
	unsigned int vsize = vm["vsize"].as<unsigned int>();
	unsigned int ksize = vm["ksize"].as<unsigned int>();


	//size_t
	unsigned int c = 0;
	SoftmaxDatasetContainer container((char *)dataset, dsize, vsize, ksize);
	SoftmaxRBMCF *rbmcf = SoftmaxRBMCFBuilder::build((char *)srbm);
	for (unsigned int d_counter = 0; d_counter < container.getLength(); d_counter++)
	{
		SoftmaxRBMCFBuilder::setDataset(*rbmcf, container[d_counter]);
		if (rbmcf->getAvailableSize() < 1) continue;

		for (unsigned int av_counter = 0; av_counter < rbmcf->getAvailableSize(); av_counter++)
		{
			unsigned int hidden_vno = rbmcf->getAvailableTable()[av_counter];
			SoftmaxRBMCF *predictrbm = SoftmaxRBMCFBuilder::duplicate(rbmcf);
			predictrbm->disableVnode(hidden_vno);
			unsigned int predict_value = predictrbm->predict(hidden_vno);
			unsigned int value = rbmcf->getVnodeValue(hidden_vno);
			double error = std::sqrt(std::pow((double)predict_value - (double)value, 2));
			std::cout << predict_value << "\t" << value << "\t" << error << std::endl;
			delete predictrbm;
		}
	}
	//std::cout << rmse.rmse << std::endl;

	return 0;
}