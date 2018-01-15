#include "SoftmaxRBMCFRMSE.h"
#include "SoftmaxDataset.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxRBM.h"
#include "SoftmaxRBMCF.h"
#include "SoftmaxRBMCFBuilder.h"
#include <boost/program_options.hpp>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
	//オプション関連
	boost::program_options::variables_map vm;
	boost::program_options::options_description option("Option");
	option.add_options()
		("srbm", boost::program_options::value<std::string>(), "srbm file")
		("dataset", boost::program_options::value<std::string>(), "dataset csv file")
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
	unsigned int vsize = vm["vsize"].as<unsigned int>();
	unsigned int ksize = vm["ksize"].as<unsigned int>();


	//size_t

	SoftmaxDatasetContainer container((char *)dataset, 1, vsize, ksize);
	SoftmaxRBMCF *rbmcf = SoftmaxRBMCFBuilder::build((char *)srbm);
	//rbmcf.setDataset(&container[0]);

	SoftmaxRBMCFRMSE rmse(*rbmcf, container[0]);

	std::cout << rmse.rmse << std::endl;

	delete rbmcf;
	return 0;
}