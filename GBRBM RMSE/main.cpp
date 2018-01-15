#include "GBRBMCFRMSE.h"
#include "GBRBMDatasetContainer.h"
#include "GBRBMCF.h"
#include "GBRBMCFBuilder.h"
#include <boost/program_options.hpp>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
	//オプション関連
	boost::program_options::variables_map vm;
	boost::program_options::options_description option("Option");
	option.add_options()
		("gbrbm", boost::program_options::value<std::string>(), "gbrbm file")
		("dataset", boost::program_options::value<std::string>(), "dataset csv file")
		("vsize", boost::program_options::value<unsigned int>(), "vsize");
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, option), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << option << std::endl;
		exit(-1);
	}

	const char *gbrbm = vm["gbrbm"].as<std::string>().data();
	const char *dataset = vm["dataset"].as<std::string>().data();
	unsigned int vsize = vm["vsize"].as<unsigned int>();


	//size_t

	GBRBMDatasetContainer container((char *)dataset, 1, vsize);
	GBRBMCF *rbmcf = GBRBMCFBuilder::build((char *)gbrbm);
	//rbmcf.setDataset(&container[0]);

	GBRBMCFRMSE rmse(*rbmcf, container[0]);

	std::cout << rmse.rmse << std::endl;

	return 0;
}