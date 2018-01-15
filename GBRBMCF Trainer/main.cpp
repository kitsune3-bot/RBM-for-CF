#include "GBRBMCFTrain.h"
#include "GBRBMDatasetContainer.h"
#include "GBRBMCF.h"
#include "GBRBMCFBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/program_options.hpp>


int main(int argc, char **argv)
{

	//オプション関連
	boost::program_options::options_description option("Option");
	option.add_options()
		("epoch,e", boost::program_options::value<unsigned int>()->default_value(50), "epoch")
		("learning_rate,l", boost::program_options::value<double>()->default_value(0.1), "learning rate[0, 1.0]")
		("cdk,c", boost::program_options::value<unsigned int>()->default_value(1), "cdk num")
		("vsize", boost::program_options::value<unsigned int>()->default_value(1), "vnode size")
		("hsize", boost::program_options::value<unsigned int>()->default_value(1), "hnode size")
		("input,i", boost::program_options::value<std::string>()->default_value("dataset.lst"), "input dataset list")
		("dsize", boost::program_options::value<unsigned int>()->default_value(1), "total number of datasets")
		("output,o", boost::program_options::value<std::string>()->default_value("out.gbrbmcf"), "output srbm name")
		("help,h", "help");
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, option), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << option << std::endl;
		exit(-1);
	}

	unsigned int epoch = vm["epoch"].as<unsigned int>();
	double learning_rate = vm["learning_rate"].as<double>();
	unsigned int cdk = vm["cdk"].as<unsigned int>();
	const char *input = vm["input"].as<std::string>().data();
	const char *output = vm["output"].as<std::string>().data();
	unsigned int vsize = vm["vsize"].as<unsigned int>();
	unsigned int hsize = vm["hsize"].as<unsigned int>();
	unsigned int dsize = vm["dsize"].as<unsigned int>();


	//データセットロード
	std::ifstream datasetifs(input);
	GBRBMDatasetContainer container((char *)input, dsize, vsize);

	GBRBMCF rbmcf(vsize, hsize);
	GBRBMCFTrain trainer(vsize, hsize, epoch, learning_rate, cdk);
	trainer.trainBatch(rbmcf, container);
	GBRBMCFBuilder::write(rbmcf, (char *)output);


	for (auto & ref : rbmcf.vbias)
	{
		std::cout << std::left << std::fixed << std::setprecision(4) << ref << ", ";
	}
	std::cout << std::endl;

	return 0;
}