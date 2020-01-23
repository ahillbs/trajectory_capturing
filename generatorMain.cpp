//
// Created by gharg on 03.04.18.
//

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include "Generator/Wrapper/GeneratorWrapper.hpp"
#include "MacroTestingFramework/InstanceGenerator.h"
#include "MacroTestingFramework/TestRawPointsContainer.h"
#include "Generator/Wrapper/GeneratorWrapper.hpp"

using  OptionDesc = boost::program_options::option_description;

struct ArgsSettings {
	std::string savePath = ".";
	std::string importPath = ".";
	uint generatorDecision = 0;
	uint iterations = 0;
	uint k = 0;
	uint maxK = 0;
	ulong m = 0;
	ulong xBound = 0;
	ulong yBound = 0;
	ulong seed = 0;
	ulong horAmount = 0;
	ulong vertAmount = 0;
	ulong instanceAmount = 20;
	ulong segIncrease = 50;
	ulong horIncrease = 5;
	ulong vertIncrease = 5;
	ulong minPoint = 0;
	ulong maxPoint = 0;
	ulong pointIncrease = 1;
	std::string name = "";


	bool parseSettings(int argc, const char* argv[]) {
		boost::program_options::options_description description;
		try {
			description.add_options()
					("minPoints,l", boost::program_options::value<ulong>(&minPoint)->default_value(1),
					 "Minimum amount of starting/shared points")
					("maxPoints,u", boost::program_options::value<ulong>(&maxPoint)->default_value(50),
					 "Maximum amount of starting/shared points")
                    ("pointIncrease", boost::program_options::value<ulong>(&pointIncrease)->default_value(1),
                     "amount of shared points increased for random points")
					("Generator,g", boost::program_options::value<unsigned int>(&generatorDecision)->default_value(1),
					 "Generator selector")
					("savePath,p", boost::program_options::value<std::string>(&savePath),
					 "Path, where instances will be saved")
					("importPath,P", boost::program_options::value<std::string>(&importPath)->default_value(""),
					 "Path, where raw data points will be imported from")
					("namePrefix,n", boost::program_options::value<std::string>(&name)->default_value(""),
					 "Name Prefix for instances")
					("segmentAmount,m", boost::program_options::value<unsigned long>(&m)->default_value(30),
					 "Amount of segments (in percent)")
					("instanceAmount,a", boost::program_options::value<unsigned long>(&instanceAmount)->default_value(60),
					 "Amount of instances created per iteration")
					("iterationAmount,I", boost::program_options::value<unsigned int>(&iterations)->default_value(8),
					 "Amount of iterations")
					("instanceIncrease,i", boost::program_options::value<unsigned long>(&segIncrease)->default_value(10),
					 "Increase of segments(percentage of connections) per iteration")
					("horAmount,h", boost::program_options::value<unsigned long>(&horAmount)->default_value(50),
					 "Amount of horizontal lines")
					("vertAmount,v", boost::program_options::value<unsigned long>(&vertAmount)->default_value(50),
					 "Amount of vertical lines")
					("xBound,x", boost::program_options::value<ulong>(&xBound)->default_value(5000),
					 "x bound for generator")
					("yBound,y", boost::program_options::value<ulong>(&yBound)->default_value(5000),
					 "y bound for generator ")
					("seed,s", boost::program_options::value<ulong>(&seed)->default_value(0),
					 "Random generator seed");


			boost::program_options::variables_map vm;
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).run(), vm);
			boost::program_options::notify(vm);
		} catch(std::exception& ex) {
			std::cerr << "Could not parse commandline: " << ex.what() << std::endl;
			std::cerr << description << std::endl;
			return false;
		}
		return true;
	}
};



template<typename G>
int generateInstances(ArgsSettings &settings,G *generator) {
	std::cout << "Settings for generator:" << std::endl;
	std::cout << generator->printSettings() << std::endl;
	std::cout << "amount of iterations: " << settings.iterations << std::endl;
	std::cout << "amount of instances per iteration: " << settings.instanceAmount << std::endl;
	TestingFramework::InstanceGenerator<G> instanceGenerator(generator);
	instanceGenerator.GenerateInstances(settings.savePath, settings.instanceAmount, settings.iterations,
										settings.name);
	return 0;
}

template<typename G>
int generateInstances(ArgsSettings &settings,const std::string &importPath, G *generator) {

	std::cout << "Settings for generator:" << std::endl;
	std::cout << generator->printSettings() << std::endl;
	std::cout << "amount of iterations: " << settings.iterations << std::endl;
	std::cout << "amount of instances per iteration: " << settings.instanceAmount << std::endl;
	std::cout << "Shared points from: " << settings.minPoint << " to " << settings.maxPoint << std::endl;

	MacroTesting::TestRawPointsContainer conti;
	if (!importPath.empty()) {
        conti.importInstances(importPath);
    } else {
	    std::cout << "Path empty. Building random point instances" << std::endl;
	    ulong n = 0;
	    for(ulong i = settings.minPoint; i < settings.maxPoint; i += settings.pointIncrease) {
            conti.addRandomPointInstance(i, settings.xBound, settings.yBound, settings.seed * 84354 + i);
            ++n;
        }
        std::cout << n << " random point instances created" << std::endl;

	}
	TestingFramework::InstanceGenerator<G> instanceGenerator(generator);
	auto rawPoints = conti.getTestInstances(settings.minPoint,settings.maxPoint,settings.instanceAmount);
	instanceGenerator.GenerateInstances(settings.savePath, rawPoints, settings.iterations, settings.name);
	return 0;
}

void translateToGenSettings(const ArgsSettings &aSettings, GeneratorCap2D::GeneratorSettings *gSettings) {
	gSettings->m = aSettings.m;
	gSettings->segIncrease = aSettings.segIncrease;
	gSettings->seed = aSettings.seed;
	gSettings->xBound = aSettings.xBound;
	gSettings->yBound = aSettings.yBound;
	gSettings->vertAmount = aSettings.vertAmount;
	gSettings->horAmount = aSettings.horAmount;
	gSettings->vertIncrease = aSettings.vertIncrease;
	gSettings->horIncrease = aSettings.horIncrease;
}

int generatorChooser(ArgsSettings settings) {
	switch (settings.generatorDecision) {
		case 1: {
			GeneratorCap2D::ProbabilisticInstanceGenerator generator;
			auto genSettings = generator.getSettings();
			translateToGenSettings(settings, &genSettings);
			generator.setSettings(genSettings);
			std::cout << "Probability shared nodes generator chosen" << std::endl;
			generateInstances(settings, settings.importPath, &generator);

			break;
		}
		case 2: {
			GeneratorCap2D::RandomSharedEndPointsInstanceGenerator generator;
			auto genSettings = generator.getSettings();
			translateToGenSettings(settings, &genSettings);
			generator.setSettings(genSettings);
			std::cout << "Random shared nodes generator chosen" << std::endl;
			generateInstances(settings, settings.importPath, &generator);

			break;
		}
		case 3: {
			GeneratorCap2D::RandomAllNodesInstanceGenerator generator;
			auto genSettings = generator.getSettings();
			translateToGenSettings(settings, &genSettings);
			generator.setSettings(genSettings);
			std::cout << "Random all nodes generator chosen" << std::endl;
			generateInstances(settings, settings.importPath, &generator);

			break;
		}
		case 4: {
			GeneratorCap2D::TwoAxisCapturingNoOverlap_Generator generator;
			auto genSettings = generator.getSettings();
			translateToGenSettings(settings, &genSettings);
			generator.setSettings(genSettings);
			std::cout << "Two axis no overlapping generator chosen" << std::endl;
			generateInstances(settings, &generator);

			break;
		}
		case 5: {
			GeneratorCap2D::TwoAxisCapturingOverlap_Generator generator;
			auto genSettings = generator.getSettings();
			translateToGenSettings(settings, &genSettings);
			generator.setSettings(genSettings);
			std::cout << "Two axis overlapping generator chosen" << std::endl;
			generateInstances(settings, &generator);

			break;
		}
		default: {
			std::cerr << "No generator specified" << std::endl;
			break;
		}
	}
	return 0;
}

int main(int argc, const char *argv[]) {

	ArgsSettings settings;
	settings.parseSettings(argc, argv);

	switch (settings.generatorDecision) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: {
			generatorChooser(settings);
			break;
		}
		default: {
			std::cout << "Nothing to generate chosen. End program" << std::endl;
			break;
		}
	}
}