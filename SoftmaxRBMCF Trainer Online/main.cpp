#include "SoftmaxRBMCFTrain.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxDataset.h"
#include "SoftmaxRBMCFBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char **argv)
{
	//using namespace boost::program_options;
	/*
	* argv[1]: epoch
	* argv[2]: learning rate
	* argv[3]: dataset name
	* argv[4]: outfile name
	* argv[5]: vlen
	* argv[6]: k
	* argv[7]: hlen
	*/

	//オプション関連
	boost::program_options::options_description option("Option");
	option.add_options()
		("epoch,e", boost::program_options::value<unsigned int>()->default_value(50), "epoch")
		("learning_rate,l", boost::program_options::value<double>()->default_value(0.1), "learning rate[0, 1.0]")
		("cdk,c", boost::program_options::value<unsigned int>()->default_value(1), "cdk num")
		("vsize", boost::program_options::value<unsigned int>()->default_value(1), "vnode size")
		("ksize", boost::program_options::value<unsigned int>()->default_value(1), "vnode k size")
		("hsize", boost::program_options::value<unsigned int>()->default_value(1), "hnode size")
		("input,i", boost::program_options::value<std::string>()->default_value("dataset.lst"), "input dataset list")
		("dsize", boost::program_options::value<unsigned int>()->default_value(1), "total number of datasets")
		("output,o", boost::program_options::value<std::string>()->default_value("out.srbm"), "output srbm name")
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
	unsigned int ksize = vm["ksize"].as<unsigned int>();
	unsigned int hsize = vm["hsize"].as<unsigned int>();
	unsigned int dsize = vm["dsize"].as<unsigned int>();
	

	SoftmaxDatasetContainer container((char *)input, dsize, 100, 5);
	SoftmaxRBMCF *rbmcf = SoftmaxRBMCFBuilder::build(vsize, hsize, ksize, container[0]);
	SoftmaxRBMCFTrain trainer(vsize, ksize, hsize, epoch, learning_rate, cdk);
	trainer.train(*rbmcf, container);
	SoftmaxRBMCFBuilder::write(*rbmcf, (char *)output);
	//std::getchar();

	return 0;
}