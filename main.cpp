#include <iostream>
#include "Utilities/GeoUtil.h"
#include "Capturing/Header/Segment.h"
#include "Capturing/Header/Capturing2D.h"
#include <boost/program_options.hpp>
#include "Visualization/SegmentVisualization.h"
#include "MacroTestingFramework/MacroTestingInstance.hpp"
#include "Solver/Wrapper/SolverWrapper.hpp"
#include "Generator/Wrapper/GeneratorWrapper.hpp"
#include "MacroTestingFramework/TestRawPointsContainer.h"
#include "Solver/Metaheuristics/EvolutionComputation.h"
#include "Solver/Metaheuristics/SimulatedAnnealing.h"

#ifndef SIMPLE_SVG_HPP
#include "Visualization/simple_svg_1.0.0.hpp"
#endif
using namespace GeneratorCap2D;
void printInstances(uint iterations, std::vector<Capturing2D> &problemInstances,
					std::vector<std::string> &problemNames) {
	for(uint i = 0; i < problemInstances.size(); ++i) {
		std::cout << "print instances" << std::endl;
		const CapturingSolution sol(&problemInstances[i]);
		problemInstances[i].printGraph(problemNames[i / iterations] + "_iteration_" + std::to_string(i % iterations)
				,sol);
	}
}

void testFirstList(std::string instancePath, ulong minPoint, ulong maxPoint, ulong maxAmount, ulong segmentPercentage,
				   uint k, uint iterations, ulong seed,
				   ulong kIncrease, ulong maxK, ulong segIncrease) {
	using namespace MacroTesting;
	Capturing2D_IP_solver solver;
	TestRawPointsContainer conti;


	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);

	//auto instances = macroTesting.getTestInstances("att48");
	solver.setCplexOutput(true);
	RandomSharedEndPointsInstanceGenerator sharedEndPointsInstanceGenerator;
	MacroTestingInstance<Capturing2D,RandomSharedEndPointsInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting(&sharedEndPointsInstanceGenerator,&solver);
	sharedEndPointsInstanceGenerator.setMaxK(k);
	sharedEndPointsInstanceGenerator.setSegmentAmount(segmentPercentage);
	sharedEndPointsInstanceGenerator.setIsRelativeSegmentAmount(true);
	auto reportRandomShared = macroTesting.runTests("RandomShared", &instances[0], instances.size(), iterations,
													k,kIncrease,maxK,segIncrease,seed);
	std::cout << reportRandomShared.to_string() << std::endl;
	MacroTesting::saveReport(".",reportRandomShared);

	//All nodes generator
	RandomAllNodesInstanceGenerator allNodesInstanceGenerator;
	allNodesInstanceGenerator.setMaxK(k);
	allNodesInstanceGenerator.setSegmentAmount(segmentPercentage);
	allNodesInstanceGenerator.setIsRelativeSegmentAmount(true);
	MacroTestingInstance<Capturing2D,RandomAllNodesInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting2(&allNodesInstanceGenerator,&solver);
	auto reportRandomAll = macroTesting2.runTests("RandomAll", &instances[0], instances.size(), iterations,k,kIncrease,maxK,segIncrease, seed);
	std::cout << reportRandomAll.to_string() << std::endl;
	MacroTesting::saveReport(".",reportRandomAll);

	//Shared Endpoint probabilistic generator run
	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting3(&probabilisticInstanceGenerator,&solver);
	auto reportProbabilisticShared = macroTesting3.runTests("ProbabilisticShared", &instances[0], instances.size(),
															   iterations,k,kIncrease,maxK,segIncrease, seed);
	std::cout << reportProbabilisticShared.to_string() << std::endl;
	MacroTesting::saveReport(".",reportProbabilisticShared);
}

void
testIPVsHeur(std::string instancePath, ulong minPoint, ulong maxPoint, ulong maxAmount, ulong segmentPercentage, uint k,
			 uint iterations, ulong seed,
			 ulong kIncrease, ulong maxK, ulong segIncrease) {
	using namespace MacroTesting;
	using namespace Metaheuristic;
	Capturing2D_IP_solver solverIP;
	Capturing2D_Greedy_solver solverGreedy;
	Capturing2D_IteratedSearch_solver solverIterated;
	EvolutionComputation solverEvo;
	SimulatedAnnealing solverSA;
	TestRawPointsContainer conti;
	//solverIP.setCplexOutput(true);
	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);

	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);

	// Evolutional Computing
	EvolveSettings evolveSettings = solverEvo.getSettings();
	evolveSettings.startPopulation = 100;
	evolveSettings.useIteratedMutation = false;
	evolveSettings.populationSize = 32;
	evolveSettings.maxNoChangedBase = 15;
	solverEvo.setSettings(evolveSettings);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,EvolutionComputation,CapturingSolution> macroTesting(&probabilisticInstanceGenerator,&solverEvo);
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed,segIncrease);


	auto problemInstances = macroTesting.getProblems();
	auto problemNames = macroTesting.getInstanceNames();
	printInstances(iterations,problemInstances,problemNames);
	// IP Solver
	solverIP.setCplexOutput(true);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTestingIP(&probabilisticInstanceGenerator,&solverIP);
	auto reportIP = macroTestingIP.runTests("solverIP", problemNames, problemInstances, instances.size(),iterations,k,kIncrease,maxK);
	std::cout << reportIP.to_string() << std::endl;
	MacroTesting::saveReport(".",reportIP);

	//Evo Solver
	auto reportEvo = macroTesting.runTests("solverEvo", problemNames, problemInstances, instances.size(), iterations,k,kIncrease,maxK);
	std::cout << reportEvo.to_string() << std::endl;
	MacroTesting::saveReport(".",reportEvo);

	// Greedy Solver
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_Greedy_solver,CapturingSolution> macroTesting2(&probabilisticInstanceGenerator,&solverGreedy);
	auto reportGreedy = macroTesting2.runTests("solverGreedy", problemNames, problemInstances, instances.size(),
												  iterations,k,kIncrease,maxK);
	std::cout << reportGreedy.to_string() << std::endl;
	MacroTesting::saveReport(".",reportGreedy);

	// Iterated Solver
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IteratedSearch_solver,CapturingSolution> macroTesting3(&probabilisticInstanceGenerator,&solverIterated);
	auto reportIterated= macroTesting3.runTests("solverIterated", problemNames, problemInstances, instances.size(),
												   iterations,k,kIncrease,maxK);
	std::cout << reportIterated.to_string() << std::endl;
	MacroTesting::saveReport(".",reportIterated);

	// SimulatedAnnealing Solver
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,SimulatedAnnealing,CapturingSolution> macroTesting4(&probabilisticInstanceGenerator,&solverSA);
	auto reportSA= macroTesting4.runTests("solverSA", problemNames, problemInstances, instances.size(),
												   iterations,k,kIncrease,maxK);
	std::cout << reportSA.to_string() << std::endl;
	MacroTesting::saveReport(".",reportSA);

}

void testThirdList(std::string instancePath, ulong minPoint, ulong maxPoint, ulong maxAmount, ulong segmentPercentage,
				   uint k, uint iterations, ulong seed,
				   ulong kIncrease, ulong maxK, ulong segIncrease) {
	using namespace MacroTesting;
	Capturing2D_IP_solver solver;
	TestRawPointsContainer conti;

	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);

	//auto instances = macroTesting.getTestInstances("att48");
	solver.setCplexOutput(true);

	//All nodes generator
	RandomAllNodesInstanceGenerator allNodesInstanceGenerator;
	allNodesInstanceGenerator.setMaxK(k);
	allNodesInstanceGenerator.setSegmentAmount(segmentPercentage);
	allNodesInstanceGenerator.setIsRelativeSegmentAmount(true);
	MacroTestingInstance<Capturing2D,RandomAllNodesInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting2(&allNodesInstanceGenerator,&solver);
	auto reportRandomAll = macroTesting2.runTests("RandomAll", &instances[0], instances.size(), iterations,k,kIncrease,maxK,segIncrease, seed);
	std::cout << reportRandomAll.to_string() << std::endl;
	MacroTesting::saveReport(".",reportRandomAll);

}

void testProbGenerator(std::string instancePath, ulong minPoint, ulong maxPoint, ulong maxAmount, ulong segmentPercentage,
				   uint k, uint iterations, ulong seed,
				   ulong kIncrease, ulong maxK, ulong segIncrease) {
	using namespace MacroTesting;
	Capturing2D_IP_solver solver;
	TestRawPointsContainer conti;

	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);

	//auto instances = macroTesting.getTestInstances("att48");
	solver.setCplexOutput(true);

	//Prob sahred nodes instance generator
	ProbabilisticInstanceGenerator ProbSharedInstanceGenerator;
	ProbSharedInstanceGenerator.setMaxK(k);
	ProbSharedInstanceGenerator.setSegmentAmount(segmentPercentage);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting(&ProbSharedInstanceGenerator,&solver);
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed,segIncrease);
	auto instancesBuild = macroTesting.getProblems();
	auto instanceNames = macroTesting.getInstanceNames();

	printInstances(iterations,instancesBuild,instanceNames);

	auto reportProbShared = macroTesting.runTests("ProbShared", instanceNames,instancesBuild, instances.size(),iterations,k,kIncrease,maxK);
	std::cout << reportProbShared.to_string() << std::endl;
	MacroTesting::saveReport(".",reportProbShared);

}

int oldmain(int argc,const char *argv[]) {

	unsigned int k = 0;
	unsigned long n = 0;
	unsigned long m = 0;
	uint problemDecision = 0;

	int xBound = 0;
	int yBound = 0;
	ulong seed = 0;
	std::string docName = "Testgraph";
	boost::program_options::options_description description;
	try {
		description.add_options()
				("TestCase,t", boost::program_options::value<unsigned int>(&problemDecision)->default_value(1),
				 "Chose Testcases 1 - 7")
				;
		description.add_options()
				("maxVertex,k", boost::program_options::value<unsigned int>(&k)->default_value(5),
				 "Maximum amount of nodes")
				("segmentAmount,m", boost::program_options::value<unsigned long>(&m)->default_value(20),
				 "Amount of segments(old Test)")
				("pointAmount,n", boost::program_options::value<unsigned long>(&n)->default_value(10),
				 "Amount of nodes (old Test)")
				("xBound,x", boost::program_options::value<int>(&xBound)->default_value(200),
				 "x bound for generator (old Test)")
				("yBound,y", boost::program_options::value<int>(&yBound)->default_value(200),
				 "y bound for generator (old Test)")
				("seed,s", boost::program_options::value<ulong>(&seed)->default_value(0),
				 "Random generator seed")
				("docname,d", boost::program_options::value<std::string>(&docName)->default_value("TestGraph"),
				 "Name of visualization document (old Test)");

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).run(), vm);
		boost::program_options::notify(vm);
	} catch(std::exception& ex) {
		std::cerr << "Could not parse commandline: " << ex.what() << std::endl;
		std::cerr << description << std::endl;
		return 1;
	}
	/*
	Segment seg1(0,0,4,4);
	Segment seg2(2,0,2,4);
	Segment seg3(1,2,6,1);
	Vector2D s1 = seg1.GetStartVector();
	Vector2D s2 = seg1.GetEndVector();

	Segment segs[] = {seg1,seg2,seg3};
	*/

	Capturing2D capObj("test","Test",0);
	//capObj.generateRandomObjects2D(m,xBound,yBound,seed);
	std::cout << "create graph..." << std::endl;
	capObj.generateRandomPoints2D(n,m,xBound,yBound,seed);
	capObj.convertSegments2Graph();
	std::cout << "graph converted" << std::endl;
	auto greedySol = capObj.solveGreedy();
	std::string greedyName = docName;
	greedyName.append("Greedy");

	double greedValue = greedySol.getSolutionValue();
	capObj.printGraph(greedyName, greedySol);
	//capObj.setCplexPreSolution(greedySol);
	auto exactSol = capObj.solveKCapturing();
	std::cout << "Value of solution: " << exactSol.getSolutionValue() << std::endl;
	std::cout << "Greedy solution value: " << greedValue << std::endl;
	capObj.printGraph(docName,exactSol);

	//std::pair<Vector2D,bool> intersectPair = GeoUtil::getIntersectionPoint(seg1,seg2);
	//std::cout << intersectPair.first.x << " " << intersectPair.first.y << " " << intersectPair.second << std::endl;
	/*
	Vector2D s1 = seg1.GetStartVector();
	Vector2D s2 = seg2.GetStartVector();
	Vector2D e1 = seg1.GetEndVector();
	Vector2D e2 = seg2.GetEndVector();

	Vector2D y;
	std::cout << s1.x << " " << s1.y << " " << e1.x << " " << e1.y << " " << s2.x << " " << s2.y << " " << e2.x << " " << e2.y << std::endl;
	double divide = (((s1.x - e1.x) * (s2.y - e2.y)) - ((s1.y - e1.y) * (s2.x - e2.x)));
	y.x = (((s1.x*e1.y - s1.y *  e1.x) * (s2.x - e2.x)) - ((s1.x - e1.x) * ( s2.x*e2.y - s2.y*e2.x))) / divide;
	y.y = (((s1.x*e1.y - s1.y *  e1.x) * (s2.y - e2.y)) - ((s1.y - e1.y) * ( s2.x*e2.y - s2.y*e2.x))) / divide;
	std::cout << y.x << " und " << y.y << " " << divide; */
	return 0;
}

void localGlobalNeighborhoodTest(uint iterations, ulong seed, std::string instancePath, ulong minPoint, ulong maxPoint,
								 ulong segmentPercentage, ulong maxAmount, uint k,
								 ulong kIncrease, ulong maxK, ulong segIncrease) {
	//using namespace Metaheuristic;

	using namespace MacroTesting;
	Capturing2D_Greedy_solver solverGreedy;
	Capturing2D_IteratedSearch_solver solverIterated;
	using namespace Metaheuristic;
	SimulatedAnnealing solverSA;
	TestRawPointsContainer conti;
	//solverIP.setCplexOutput(true);
	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);
	std::cout << "amount instances: " << instances.size() << " max amount " << maxAmount << std::endl;
	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);



	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_Greedy_solver,CapturingSolution> macroTesting(&probabilisticInstanceGenerator,&solverGreedy);
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed,segIncrease);
	auto problemInstances = macroTesting.getProblems();
	auto problemNames = macroTesting.getInstanceNames();
	printInstances(iterations,problemInstances,problemNames);

	auto greedyReport = macroTesting.runTests("solverGreedyLocalGlobal", problemNames, problemInstances, instances.size(),
											  iterations,k,kIncrease,maxK);
	solverIterated.setLocalSearch(false);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IteratedSearch_solver,CapturingSolution> macroTestingIS(&probabilisticInstanceGenerator,&solverIterated);
	auto iteratedReport = macroTestingIS.runTests("solverIteratedGlobal", problemNames, problemInstances, instances.size(),
											  iterations,k,kIncrease,maxK);
	solverIterated.setLocalSearch(true);
	auto iteratedLocalReport = macroTestingIS.runTests("solverIteratedLocal", problemNames, problemInstances, instances.size(),
												  iterations,k,kIncrease,maxK);


	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,SimulatedAnnealing,CapturingSolution> macroTestingSA(&probabilisticInstanceGenerator,&solverSA);
	solverSA.setUseLocal(false);
	auto SAReport = macroTestingSA.runTests("solverSAGlobal", problemNames, problemInstances, instances.size(),
											  iterations,k,kIncrease,maxK);
	solverSA.setUseLocal(true);
	auto SAReportLocal = macroTestingSA.runTests("solverSALocal", problemNames, problemInstances, instances.size(),
											iterations,k,kIncrease,maxK);
	saveReport(".",greedyReport);
	saveReport(".",iteratedReport);
	saveReport(".",iteratedLocalReport);
	saveReport(".",SAReport);
	saveReport(".",SAReportLocal);
}

void iteratedVsRandomEATest(uint iterations, ulong seed, std::string instancePath, ulong minPoint, ulong maxPoint,
							ulong segmentPercentage, ulong maxAmount, uint k,
		ulong kIncrease, ulong maxK, ulong segIncrease) {
	//using namespace Metaheuristic;
	using namespace MacroTesting;
	using namespace Metaheuristic;
	TestRawPointsContainer conti;
	//solverIP.setCplexOutput(true);
	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);
	std::cout << "amount instances: " << instances.size() << " max amount " << maxAmount << std::endl;
	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);

	EvolutionComputation evolutionComputation;
	Capturing2D_Greedy_solver solverGreedy;

	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_Greedy_solver,CapturingSolution> macroTesting(&probabilisticInstanceGenerator,&solverGreedy);
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed,segIncrease);
	auto problemInstances = macroTesting.getProblems();
	auto problemNames = macroTesting.getInstanceNames();
	printInstances(iterations,problemInstances,problemNames);

	auto greedyReport = macroTesting.runTests("solverGreedy", problemNames, problemInstances, instances.size(),
											  iterations,k,kIncrease,maxK);

	EvolveSettings settings = evolutionComputation.getSettings();
	settings.startPopulation = 100;
	settings.useIteratedMutation = false;
	settings.populationSize = 32;
	settings.maxNoChangedBase = 15;
	settings.useIteratedMutation = false;
	evolutionComputation.setSettings(settings);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,EvolutionComputation,CapturingSolution> macroTestingEA(&probabilisticInstanceGenerator,&evolutionComputation);

	auto EAReport = macroTestingEA.runTests("solverEAIterated", problemNames, problemInstances, instances.size(),
											iterations,k,kIncrease,maxK);
	settings.useIteratedMutation = (true);
	evolutionComputation.setSettings(settings);
	auto EAReportRandom = macroTestingEA.runTests("solverEARandom", problemNames, problemInstances, instances.size(),
												 iterations,k,kIncrease,maxK);
	saveReport(".",greedyReport);
	saveReport(".",EAReportRandom);
	saveReport(".",EAReport);
}



void printInstances(uint iterations, ulong seed, std::string instancePath, ulong minPoint, ulong maxPoint,
					ulong segmentPercentage, ulong maxAmount, uint k,
					ulong kIncrease, ulong maxK, ulong segIncrease) {

	using namespace MacroTesting;
	using namespace Metaheuristic;
	TestRawPointsContainer conti;
	//solverIP.setCplexOutput(true);

	EvolutionComputation solverEvo;
	conti.importInstances(instancePath);
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(minPoint, maxPoint, maxAmount);

	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);

	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,EvolutionComputation,CapturingSolution> macroTesting(&probabilisticInstanceGenerator,&solverEvo);
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed,segIncrease);
	auto problemInstances = macroTesting.getProblems();
	auto problemNames = macroTesting.getInstanceNames();
	printInstances(iterations, problemInstances, problemNames);
}

int main(int argc, const char *argv[]) {
	uint problemDecision = 1;
	uint k = 5;
	uint iterations = 5;
	ulong seed = 50;
	std::string instancePath = "/home/gharg/Downloads/hip_instances";
	ulong minPoint = 50;
	ulong maxPoint = 55;
	ulong maxInstanceAmount = 10;
	ulong segPercent = 20;
	ulong kIncrease = 5;
	ulong maxK = 30;
	ulong segIncrease = 5;
	boost::program_options::options_description description;
	try {
		description.add_options()
				("TestCase,t", boost::program_options::value<unsigned int>(&problemDecision)->default_value(1),
				 "Chose test cases")
				;
		description.add_options()
				("maxVertex,k", boost::program_options::value<unsigned int>(&k)->default_value(5),
				 "Maximum amount of nodes")
				("seed,s", boost::program_options::value<ulong>(&seed)->default_value(50), "Seed für den Random Generator")
				;
		description.add_options()
				("minPoints,l", boost::program_options::value<ulong>(&minPoint)->default_value(50),
				 "Minimum amount of starting/shared points")
				("maxPoints,u", boost::program_options::value<ulong>(&maxPoint)->default_value(55),
				 "Maximum amount of starting/shared points")
				("maxAmount,a", boost::program_options::value<ulong>(&maxInstanceAmount)->default_value(0),
				 "Maximum amount of instances")
				("SegPercent,c", boost::program_options::value<ulong>(&segPercent)->default_value(20),
				 "Amount of segments in percent relative to maximal amount possible")
				("Iterations,i", boost::program_options::value<uint>(&iterations)->default_value(5),
				 "Amount of iterations per instance")
				("Path,p", boost::program_options::value<std::string>(&instancePath)->default_value("."), "Pfad zu den Instanzen")
				("segIncrease,x", boost::program_options::value<ulong>(&segIncrease)->default_value(5),
				 "Percentage increase of segments")
				("maxK,K", boost::program_options::value<ulong>(&maxK)->default_value(30),
				 "Maximum K")
				("kIncrease,v", boost::program_options::value<ulong>(&kIncrease)->default_value(5),
				 "Increase of solution nodes taken per run");
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).run(), vm);
		boost::program_options::notify(vm);

	} catch(std::exception& ex) {
		std::cerr << "Could not parse commandline: " << ex.what() << std::endl;
		std::cerr << description << std::endl;
		return 1;
	}
	switch (problemDecision) {
		case 1: {
			testFirstList(instancePath, minPoint, maxPoint, maxInstanceAmount, segPercent, k, iterations, seed,
						  kIncrease, maxK, segIncrease);
			break;
		}
		case 2: {
			testIPVsHeur(instancePath, minPoint, maxPoint, maxInstanceAmount, segPercent, k, iterations, seed,
						 kIncrease, maxK, segIncrease);
			break;
		}
		case 3:
			testThirdList(instancePath, minPoint, maxPoint, maxInstanceAmount, segPercent, k, iterations, seed,
						  kIncrease, maxK, segIncrease);
			break;
		case 4:
			localGlobalNeighborhoodTest(iterations, seed, instancePath, minPoint, maxPoint, segPercent,
										maxInstanceAmount, k, kIncrease, maxK, segIncrease);
			break;
		case 5:
			iteratedVsRandomEATest(iterations, seed, instancePath, minPoint, maxPoint, segPercent, maxInstanceAmount, k,
								   kIncrease, maxK, segIncrease);
			break;
		case 6:
			printInstances(iterations, seed, instancePath, minPoint, maxPoint, segPercent, maxInstanceAmount, k,
								   kIncrease, maxK, segIncrease);
			break;
		case 7:
			testProbGenerator(instancePath, minPoint, maxPoint, maxInstanceAmount, segPercent, k, iterations, seed,
							  kIncrease, maxK, segIncrease);
			break;
		default:
			oldmain(argc,argv);
			break;
	}

}
