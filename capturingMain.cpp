//
// Created by gharg on 15.02.18.
//

#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include "MacroTestingFramework/MacroTestingInstance.hpp"
#include "Generator/Header/Cap2DGenerator.h"
#include "Generator/Wrapper/GeneratorWrapper.hpp"
#include "Solver/Solver/AllEdgesIPSolver.h"
#include "Solver/Solver/ThesisIPSolver.h"
#include "MacroTestingFramework/TestRawPointsContainer.h"
#include "MacroTestingFramework/CapturingInstanceContainer.h"



using  OptionDesc = boost::program_options::option_description;
namespace Testing {
    struct ArgsSettings {
        uint generatorDecision = 0;
        uint k = 0;
        uint maxK = 0;
        //ulong m = 0;
        //ulong xBound = 0;
        //ulong yBound = 0;
        ulong seed = 0;
        ulong minVert = 0;
        ulong maxVert = 0;
        //ulong instanceAmount = 20;
        //ulong segIncrease = 50;
        //ulong horIncrease = 5;
        //ulong vertIncrease = 5;
        uint kIncrease = 5;
        //ulong segPercent;
        ulong segMin = 1000;
        ulong segMax = 500;
        ulong sharedPointMin = 00;
        ulong sharedPointMax = 50;
        std::vector<std::string> instancePaths;
        ulong maxInstances = 0;
        ulong skipInstances = 0;
        bool recursive = false;
        std::string reportSuffix = "";
        std::string savePath = ".";
        std::string instanceNameFilter;
        bool saveSolutions;

        bool parseSettings(int argc, const char *argv[]) {
            boost::program_options::options_description description;
            try {
                description.add_options()
                        ("TestCase,t",
                         boost::program_options::value<unsigned int>(&generatorDecision)->default_value(4),
                         "Test cases selection")
                        ("minVertex,k", boost::program_options::value<unsigned int>(&k)->default_value(5),
                         "Minimum/starting amount of nodes a solution can take")
                        ("maxVertex,K", boost::program_options::value<unsigned int>(&maxK)->default_value(26),
                         "Maximum/end amount of nodes a solution can take")
                        ("increaseK", boost::program_options::value<unsigned int>(&kIncrease)->default_value(3),
                         "K increase amount for nodes a solution can take")
                        //("instanceAmount,I",
                         //boost::program_options::value<unsigned long>(&instanceAmount)->default_value(20),
                         //"Amount of instances created")
                        ("seed,s", boost::program_options::value<ulong>(&seed)->default_value(0),
                         "Random generator seed")
                        ("segMin,m", boost::program_options::value<ulong>(&segMin)->default_value(0),
                         "segment minimum")
                        ("segMax,M", boost::program_options::value<ulong>(&segMax)->default_value(0),
                         "segment maximum")
                        ("sharedMin,r", boost::program_options::value<ulong>(&sharedPointMin)->default_value(0),
                         "Shared endpoints minimum")
                        ("sharedMax,R", boost::program_options::value<ulong>(&sharedPointMax)->default_value(60),
                         "Shared endpoints maximum")
                        ("skipInstance", boost::program_options::value<ulong>(&skipInstances)->default_value(0),
                         "Skip the first X instances found")
						("maxInstances,a", boost::program_options::value<ulong>(&maxInstances)->default_value(0),
						 "Maximum amount of instances used. 0 = inf")
                        ("minVert,v", boost::program_options::value<ulong>(&minVert)->default_value(0000),
                         "Minimum amount of Vertices used. 0 = inf")
                        ("maxVert,V", boost::program_options::value<ulong>(&maxVert)->default_value(12000),
                         "Maximum amount of Vertices  used. 0 = inf")
                        ("instancePaths,p", boost::program_options::value<std::vector<std::string>>(&instancePaths),
                         "Path to instances/metaData")
                        ("recursive", boost::program_options::value<bool>(&recursive)->default_value(true),
                         "Subdirectories will also be evaluated")
                        ("nameSuffix", boost::program_options::value<std::string>(&reportSuffix)->default_value(""),
                         "Name suffix for generated reports. If empty the time will be appended")
                        ("savePath,S", boost::program_options::value<std::string>(&savePath)->default_value("."),
                         "Path where the reports will be saved")
                        ("nameFilter", boost::program_options::value<std::string>(&instanceNameFilter)->default_value(""),
                         "Filter for the instance names")
                        ("saveSolutions", boost::program_options::value<bool>(&saveSolutions)->default_value(false),
                        "Saves svgs with solutions at the save path");

                boost::program_options::variables_map vm;
                boost::program_options::store(
                        boost::program_options::command_line_parser(argc, argv).options(description).run(), vm);
                boost::program_options::notify(vm);
            } catch (std::exception &ex) {
                std::cerr << "Could not parse commandline: " << ex.what() << std::endl;
                std::cerr << description << std::endl;
                return false;
            }
            return true;
        }
    };
}

/* --------------- function declaration ---------------------------------------- */
std::vector<CapturingMetaData> fillContainer(const Testing::ArgsSettings &settings,
                                                   TestingFramework::CapturingInstanceContainer *pContainer);


/* ------------------ functions --------------------------------------------------- */
void appendTime(std::string* name) {
    std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());

    std::time_t t = s.count();
    //name->append("_");
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t),"_%F_%T");
    name->append(ss.str());
}

void OldIPTest(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {

    ThesisIPSolver IPSolver;

    auto reportIP = MacroTesting::runTests("IPTime",metaData,IPSolver,settings.k,settings.kIncrease,settings.maxK);

    if(settings.reportSuffix.empty()) {
        appendTime(&reportIP.name);
    }
    std::cout << reportIP.to_string() << std::endl;
    MacroTesting::saveReport(settings.savePath,reportIP);
    std::cout << "save solution:" << settings.saveSolutions << std::endl;
    if(settings.saveSolutions) {
        MacroTesting::saveSolution(settings.savePath,reportIP);
    }
}

void NewIPTest(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {


    AllEdgesIPSolver newIPSolver;

    auto reportNewIP = MacroTesting::runTests("NewIPTime",metaData,newIPSolver,settings.k,settings.kIncrease,
                                              settings.maxK);
    std::cout << reportNewIP.to_string() << std::endl;


    if(settings.reportSuffix.empty()) {

        appendTime(&reportNewIP.name);
    }
    MacroTesting::saveReport(".",reportNewIP);


}

void LPTest(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {

    ThesisIPSolver LPSolver;

    auto lPSettings = LPSolver.getSettings();
    lPSettings.relaxation = true;
    LPSolver.setSettings(lPSettings);




    auto reportLP = MacroTesting::runTests("LPTime",metaData,LPSolver,settings.k,settings.kIncrease,
                                           settings.maxK);
    if(settings.reportSuffix.empty()) {
        appendTime(&reportLP.name);
    }
    std::cout << reportLP.to_string() << std::endl;
    MacroTesting::saveReport(settings.savePath,reportLP);
}

void LPvsIP(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {


	ThesisIPSolver IPSolver;
	ThesisIPSolver LPSolver;

	auto lPSettings = LPSolver.getSettings();
	lPSettings.relaxation = true;
	LPSolver.setSettings(lPSettings);



	auto reportIP = MacroTesting::runTests("IPTime",metaData,IPSolver,settings.k,settings.kIncrease,settings.maxK);

	auto reportLP = MacroTesting::runTests("LPTime",metaData,LPSolver,settings.k,settings.kIncrease,
														  settings.maxK);
	if(settings.reportSuffix.empty()) {
        appendTime(&reportIP.name);
        appendTime(&reportLP.name);
    }
	std::cout << reportIP.to_string() << std::endl;
	MacroTesting::saveReport(settings.savePath,reportIP);

	std::cout << reportLP.to_string() << std::endl;
	MacroTesting::saveReport(settings.savePath,reportLP);
}

void LPOldVsNew(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {
    ThesisIPSolver LPSolver;
    AllEdgesIPSolver newLPSolver;


    auto lPSettings = LPSolver.getSettings();
    lPSettings.relaxation = true;
    LPSolver.setSettings(lPSettings);

    auto newLpSettings = newLPSolver.getSettings();
    newLpSettings.relaxation = true;
    newLPSolver.setSettings(newLpSettings);

    auto reportNewLP = MacroTesting::runTests("NewLPTime",metaData,newLPSolver,settings.k,settings.kIncrease,settings.maxK);

    auto reportLP = MacroTesting::runTests("LPTime",metaData,LPSolver,settings.k,settings.kIncrease,
                                           settings.maxK);
    if(settings.reportSuffix.empty()) {
        appendTime(&reportNewLP.name);
        appendTime(&reportLP.name);
    }
    std::cout << reportNewLP.to_string() << std::endl;
    MacroTesting::saveReport(settings.savePath,reportNewLP);

    std::cout << reportLP.to_string() << std::endl;
    MacroTesting::saveReport(settings.savePath,reportLP);
}

void relaxedEdges(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {


	ThesisIPSolver IPSolver;
	ThesisIPSolver LPSolver;

	auto lPSettings = LPSolver.getSettings();
	lPSettings.edgeRelaxation = true;
	LPSolver.setSettings(lPSettings);



	auto reportIP = MacroTesting::runTests("StandardIPTime",metaData,IPSolver,settings.k,settings.kIncrease,settings.maxK);

	auto reportLP = MacroTesting::runTests("RelaxedEdgesIPTime",metaData,LPSolver,settings.k,settings.kIncrease,
										   settings.maxK);


    if(settings.reportSuffix.empty()) {
        appendTime(&reportIP.name);
        appendTime(&reportLP.name);
    }
	std::cout << reportIP.to_string() << std::endl;
	MacroTesting::saveReport(settings.savePath,reportIP);

	std::cout << reportLP.to_string() << std::endl;
	MacroTesting::saveReport(settings.savePath,reportLP);
}

void oldVsNewIP(Testing::ArgsSettings &settings,std::vector<CapturingMetaData> &metaData) {


	AllEdgesIPSolver newIPSolver;
	ThesisIPSolver oldIPSolver;


	auto reportNewIP = MacroTesting::runTests("NewIPTime",metaData,newIPSolver,settings.k,settings.kIncrease,
                                              settings.maxK);
    std::cout << reportNewIP.to_string() << std::endl;

    auto reportOldIP = MacroTesting::runTests("OldIPTime",metaData,oldIPSolver,settings.k,settings.kIncrease,
                                              settings.maxK);


    if(settings.reportSuffix.empty()) {
        appendTime(&reportOldIP.name);
        appendTime(&reportNewIP.name);
    }
    MacroTesting::saveReport(".",reportNewIP);
    std::cout << reportOldIP.to_string() << std::endl;
	MacroTesting::saveReport(".",reportOldIP);

}

std::vector<CapturingMetaData> fillContainer(const Testing::ArgsSettings &settings,
                                                   TestingFramework::CapturingInstanceContainer *pContainer) {
	for(auto& path : settings.instancePaths) {
		pContainer->addInstances(path, settings.recursive);
	}
	return pContainer->getMetaInstances(settings.maxInstances, settings.segMin, settings.segMax, settings.minVert,
                                        settings.maxVert, settings.sharedPointMin, settings.sharedPointMax,
                                        settings.instanceNameFilter, settings.skipInstances);
}

int main(int argc, const char *argv[]) {

    Testing::ArgsSettings settings;
    settings.parseSettings(argc, argv);

    TestingFramework::CapturingInstanceContainer container;
    std::vector<CapturingMetaData> metaData = fillContainer(settings,&container);
    std::cout << "container: " << container.getContainerSize() << std::endl;
    std::cout << "metaData: " << metaData.size() << std::endl;
	switch (settings.generatorDecision) {
		case 1:
            oldVsNewIP(settings,metaData);
			break;
        case 2:
            //LPvsIP(settings);
			LPvsIP(settings,metaData);
            break;
		case 3:
            relaxedEdges(settings,metaData);
			break;
		case 4:
		    OldIPTest(settings,metaData);
			break;
		case 5:
            LPTest(settings,metaData);
            break;
        case 6:
            NewIPTest(settings,metaData);
            break;
	    case 7:
	        LPOldVsNew(settings,metaData);
		case 0:
            break;
        default:
			break;
	}

}

