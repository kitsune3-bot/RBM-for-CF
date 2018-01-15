#include "SoftmaxRBMAvg.h"
#include "SoftmaxDataset.h"
#include "SoftmaxDatasetContainer.h"
#include "SoftmaxRBM.h"
#include "SoftmaxRBMCF.h"
#include "SoftmaxRBMCFBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

int main(int argc, char **argv)
{
	boost::program_options::options_description option("Option");
	option.add_options()
		("rbmlist", boost::program_options::value<std::string>(), "rbm list file")
		("datasetlist", boost::program_options::value<std::string>(), "dataset list file")
		("listsize", boost::program_options::value<unsigned int>(), "sizeof rbm list")
		("output", boost::program_options::value<std::string>(), "avgrbm output fname")
		("vsize", boost::program_options::value<unsigned int>(), "vnode size")
		("ksize", boost::program_options::value<unsigned int>(), "vnode k size")
		("hsize", boost::program_options::value<unsigned int>(), "hnode size")
		("help,h", "help");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, option), vm);
	boost::program_options::notify(vm);

	if ( !vm.count("rbmlist") || !vm.count("datasetlist") || !vm.count("listsize")
		|| !vm.count("output") || !vm.count("vsize") || !vm.count("ksize")
		|| !vm.count("hsize") || vm.count("help") ) {

		std::cout << option << std::endl;
		exit(-1);
	}


	std::string rbmlist = vm["rbmlist"].as<std::string>();
	std::string datasetlist = vm["datasetlist"].as<std::string>();
	unsigned int listsize = vm["listsize"].as<unsigned int>();
	std::string output = vm["output"].as<std::string>();
	unsigned int vsize = vm["vsize"].as<unsigned int>();
	unsigned int ksize = vm["ksize"].as<unsigned int>();
	unsigned int hsize = vm["hsize"].as<unsigned int>();

	std::ifstream rbmifs(rbmlist);
	std::ifstream datasetifs(datasetlist);
	SoftmaxRBMAvg avg(vsize, hsize, ksize);

	for (unsigned int i = 0; i < listsize; i++)
	{
		char rbmbuff[0xff];
		char datasetbuff[0xff];

		rbmifs.getline(rbmbuff, 0xff);
		datasetifs.getline(datasetbuff, 0xff);

		SoftmaxDatasetContainer container(datasetbuff, 1, vsize, ksize);
		SoftmaxRBMCF *rbm = SoftmaxRBMCFBuilder::build(rbmbuff);
		SoftmaxRBMCFBuilder::setDataset(*rbm, container[0]);
		avg.addParameters(rbm);

		delete rbm;
	}

	SoftmaxRBMCF *out_rbm = avg.getAvgRBMCF();
	SoftmaxRBMBuilder::write(*out_rbm, (char *)output.data());


	return 0;
}