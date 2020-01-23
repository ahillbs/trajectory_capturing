//
// Created by gharg on 19.09.17.
//
 /*
#include <gtest/gtest.h>
#include "../MacroTestingFramework/MacroTestingInstance.hpp"
#include "../Capturing/Header/Capturing2D.h"
#include "../Generator/Wrapper/GeneratorWrapper.hpp"
#include "../Solver/Wrapper/SolverWrapper.hpp"
#include "../MacroTestingFramework/TestRawPointsContainer.h"
#include "../Solver/Metaheuristics/SimulatedAnnealing.h"
#include "../Solver/Metaheuristics/EvolutionComputation.h"
#include "../Solver/Solver/GreedySolver.h"
#include "../MacroTestingFramework/CapturingInstanceContainer.h"
#include "../Import/Imports.h"

  */

/*
using namespace MacroTesting;
using namespace GeneratorCap2D;
TEST(TestFrameworkTests,csvImportTest) {
	std::cout << std::endl << "Start testing csvImport for Test Framework:" << std::endl;
	TestRawPointsContainer conti;
	MacroTestingInstance<Capturing2D,RandomSharedEndPointsInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting;
	conti.importInstances("/home/gharg/Downloads/hip_instances");
	auto instances = conti.getTestInstances("att");
	for (uint i = 0; i < instances.size(); ++i) {
		std::cout << "instances: " << instances[i]->name << std::endl;
		for (uint j = 0; j < instances[i]->points[0].size(); ++j) {
			std::cout << "x: " << instances[i]->points[0][j] << " y: " << instances[i]->points[1][j] << std::endl;
		}
	}
}
void outputReport(Report report, std::ostream& ostream = std::cout) {
	const char* separator = "\t";
	ostream << "output: " << report.name << std::endl;
	for(auto headerName : report.header) {
		ostream << headerName << separator;
	}
	ostream << std::endl;
	for (auto reportRow : report.data) {
		for (auto reportCell : reportRow) {
			ostream << reportCell << separator;
		}
		ostream << std::endl;
	}
}

TEST(GreedyTests,NewVsOldGreedy) {
	std::cout << std::endl << "Start testing old vs new greedy with Test Framework:" << std::endl;

	GreedySolver solverNew;
	Capturing2D_Greedy_solver solverGreedy;

	uint k = 5;
	uint increaseK = 5;
	uint maxK = 25;


	TestRawPointsContainer conti;

	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,GreedySolver,CapturingSolution> macroTesting(&probabilisticInstanceGenerator,&solverNew);
	conti.importInstances("/Data/git/ba-hill/hip_instances/");
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(48, 48);

	probabilisticInstanceGenerator.setProbability(20);

	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),1,0,0);
	auto problem = macroTesting.getProblems();
	auto problemName = macroTesting.getInstanceNames();

	auto newGreedyReport = macroTesting.runTests("NewGreedy",problemName,problem,problem.size(),1,k,increaseK,maxK);

	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_Greedy_solver,CapturingSolution> macroTestingOldGreedy(&probabilisticInstanceGenerator,&solverGreedy);
	auto oldGreedyReport = macroTestingOldGreedy.runTests("OldGreedy",problemName,problem,problem.size(),1,k,increaseK,maxK);

	std::vector<std::string> valueStringsNew;
	for (auto vec : newGreedyReport.data) {
		valueStringsNew.push_back(vec[7]);
	}

	std::vector<std::string> valueStringsOld;
	for (auto vec : oldGreedyReport.data) {
		valueStringsOld.push_back(vec[7]);
	}

	std::cout << "----------------------------------------- NEW GREEDY RESULTS ------------------------------------------------" << std::endl;

	std::cout << newGreedyReport.to_string() << std::endl;

	std::cout << "----------------------------------------- OLD GREEDY RESULTS ------------------------------------------------" << std::endl;

	std::cout << oldGreedyReport.to_string() << std::endl;

	std::cout << "----------------------------------------- VALUE COMPARISON GREEDY RESULTS ------------------------------------------------" << std::endl;

	std::cout << " Value New" << "\t" << "Value Old" << std::endl;
	for (ulong j = 0; j < valueStringsOld.size(); ++j) {
		std::cout << valueStringsNew[j] << "\t" << valueStringsOld[j] << std::endl;
	}

	ASSERT_EQ(valueStringsNew,valueStringsOld);
}

TEST(ImportInstancesTests,ImportOneInstanceTest) {
	using namespace TestingFramework;
	CapturingInstanceContainer conti;
	conti.addInstances("TestSavePC/ProbabilisticInstance", false);
    std::cout << "Found " << conti.getContainerSize() << " instances" << std::endl;
	auto metaInstances = conti.getMetaInstances(0, 0, 0, 0, 0, 0, 0, "berlin");
	if(metaInstances.size() > 0) {
		std::cout << "Found instance. Convert to instance" << std::endl;
		CapturingMetaData* meta = metaInstances[0];
		Capturing2D instance = conti.getInstance(metaInstances[0]);
		instance.convertSegments2Graph();

		ASSERT_EQ(instance.getVectorFamilies().size(),meta->segmentAmount);
        ASSERT_EQ(instance.getGraph()->getVertices().size(),meta->vAmount);
	}
}

TEST(ImportInstanceTests,HashTest) {



	// Test hash
	std::string hash = Imports::hashfile("TestSavePC/ProbabilisticInstance/berlin52_Prob_S0_F261.svg");
	std::cout << "The hash of our file is: " << hash << std::endl;


	std::string hash2 = Imports::hashfile("TestSavePC/ProbabilisticInstance/skyline_ontario-edges-50-float-42_Prob_S0_F241.svg");
	std::cout << "The hash of our second file is: " << hash2 << std::endl;
}

TEST(TestFrameworkTests,MacroTestintTest) {

	std::cout << std::endl << "Start testing macro test for Test Framework:" << std::endl;
	Capturing2D_IP_solver solver;
	uint k = 120;
	uint iterations = 5;
	ulong seed = 50;
	TestRawPointsContainer conti;

	RandomSharedEndPointsInstanceGenerator sharedEndPointsInstanceGenerator;
	MacroTestingInstance<Capturing2D,RandomSharedEndPointsInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting(&sharedEndPointsInstanceGenerator,&solver);
	conti.importInstances("/home/gharg/Downloads/hip_instances");
	std::vector<TestRawPoints<NT> *> instances;
	instances = conti.getTestInstances(48, 48);

	//auto instances = macroTesting.getTestInstances("att48");

	sharedEndPointsInstanceGenerator.setMaxK(k);
	sharedEndPointsInstanceGenerator.setSegmentAmount(20);
	sharedEndPointsInstanceGenerator.setIsRelativeSegmentAmount(true);
	auto reportRandomShared = macroTesting.runAvgTests("RandomShared", &instances[0], instances.size(), iterations,
													   seed);
	outputReport(reportRandomShared);
	MacroTesting::saveReport(".",reportRandomShared);

	//All nodes generator
	RandomAllNodesInstanceGenerator allNodesInstanceGenerator;
	allNodesInstanceGenerator.setMaxK(k);
	allNodesInstanceGenerator.setSegmentAmount(20);
	allNodesInstanceGenerator.setIsRelativeSegmentAmount(true);
	MacroTestingInstance<Capturing2D,RandomAllNodesInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting2(&allNodesInstanceGenerator,&solver);
	auto reportRandomAll = macroTesting2.runAvgTests("RandomAll", &instances[0], instances.size(), iterations, seed);
	outputReport(reportRandomAll);
	MacroTesting::saveReport(".",reportRandomAll);

	//Shared Endpoint probabilistic generator run
	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(20);
	MacroTestingInstance<Capturing2D,ProbabilisticInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting3(&probabilisticInstanceGenerator,&solver);
	auto reportProbabilisticShared = macroTesting3.runAvgTests("ProbabilisticShared", &instances[0], instances.size(),
															   iterations, seed);
	outputReport(reportProbabilisticShared);
	MacroTesting::saveReport(".",reportProbabilisticShared);
}

TEST(TestFrameworkTests,solverTest) {
	std::cout << std::endl;
	TestRawPointsContainer conti;
	conti.importInstances("/Data/git/ba-hill/hip_instances/");
	auto instances = conti.getTestInstances("random-2-opt-50-1337");
	NT offsetX = 0;
	NT offsetY = 0;
	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
		if(instances[0]->points[0][i] <= offsetX) {
			offsetX = instances[0]->points[0][i];
		}
		if(instances[0]->points[1][i] <= offsetY) {
			offsetY = instances[0]->points[1][i];
		}
	}
	std::cout << "offsetX before: " << offsetX << std::endl;
	std::cout << "offsetY before: " << offsetY << std::endl;
	//offsetX = std::round(-offsetX) + 1;
	//offsetY = std::round(-offsetY) + 1;
	std::cout << "offsetX after: " << offsetX << std::endl;
	std::cout << "offsetY after: " << offsetY << std::endl;
	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
		instances[0]->points[0][i] = instances[0]->points[0][i] + offsetX;
		instances[0]->points[1][i] = instances[0]->points[1][i] + offsetY;
	}

	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
		if(instances[0]->points[0][i] <= 0) {
			std::cout << "lower x: " << instances[0]->points[0][i] << std::endl;
			offsetX = instances[0]->points[0][i];
		}
		if(instances[0]->points[1][i] <= 0) {
			std::cout << "lower y: " << instances[0]->points[1][i] << std::endl;
			offsetX = instances[0]->points[1][i];
		}
	}

	std::cout << "instances: " << instances.size() << std::endl;
	uint k = 5;
	uint segmentPercentage = 20;
	uint iterations = 1;
	uint seed = 10;

	Capturing2D_IP_solver solverIP;
	Capturing2D_Greedy_solver solverGreedy;
	Capturing2D_IteratedSearch_solver solverIterated;

	RandomSharedEndPointsInstanceGenerator sharedEndPointsInstanceGenerator;
	sharedEndPointsInstanceGenerator.setMaxK(k);
	sharedEndPointsInstanceGenerator.setIsRelativeSegmentAmount(true);
	sharedEndPointsInstanceGenerator.setSegmentAmount(segmentPercentage);

	ProbabilisticInstanceGenerator probabilisticInstanceGenerator;
	probabilisticInstanceGenerator.setMaxK(k);
	probabilisticInstanceGenerator.setProbability(segmentPercentage);
	std::cout << "build macroTesting" << std::endl;
	MacroTestingInstance<Capturing2D,RandomSharedEndPointsInstanceGenerator,Capturing2D_IP_solver,CapturingSolution> macroTesting(&sharedEndPointsInstanceGenerator,&solverIP);
	std::cout << "generate instance...";
	macroTesting.bulkBuildProblemInstances(&instances[0],instances.size(),iterations,seed);
	std::cout << " finished" << std::endl;
	auto problemInstances = macroTesting.getProblems();
	if(problemInstances.size() > 0) {
		Capturing2D problem = problemInstances[0];

		CapturingSolution GreedySol = solverGreedy.solve(&problem,k);
		std::cout << "Solution Greedy: " << GreedySol.getSolutionValue() << std::endl;
		CapturingSolution IteratedSol = solverIterated.solve(&problem,k);
		std::cout << "Solution Iterated: " << IteratedSol.getSolutionValue() << std::endl;
		std::cout << "Computed solution Iterated: " << problem.computeSolution(IteratedSol.getVertexSolution()).getSolutionValue() << std::endl;
		std::cout << "Solution Iterated is feasible: " << IteratedSol.checkFeasible() << std::endl;
		std::cout << "Segments amount: " << problem.getVertexFamilies()->size() << std::endl;
		std::cout << "Edges amount: " << problem.getGraph()->getEdges().size() << std::endl;
		std::string docIPname = "SolIP";
		std::string docGreedyname = "SolGreedy";
		std::string docIteratedname = "SolIterated";
		std::set<Vertex> preSolVert = {502,549,1539,1581,4476};
		std::set<Vertex> IteratedSolpre = {500,1651,2354,3184,6030};
		CapturingSolution preSol = problem.computeSolution(preSolVert);
		std::cout << "Solution preSolved: " << preSol.getSolutionValue() << std::endl;
		
		problem.setCplexPreSolution(IteratedSol);
		//problem.setCplexPreSolution(preSol);
		solverIP.setCplexOutput(true);
		CapturingSolution IPSol = solverIP.solve(&problem,k);
		std::cout << "Solution IP: " << IPSol.getSolutionValue() << std::endl;
		problem.printGraph(docIPname,IPSol);
		problem.printGraph(docGreedyname,GreedySol);
		problem.printGraph(docIteratedname,IteratedSol);
		auto compSol = problem.computeSolution(IPSol.getVertexSolution());
		std::cout << "Computed IP Solution: " << compSol.getSolutionValue() << std::endl;
		auto& vertSol = compSol.getVertexSolution();
		std::cout << "IP solution: ";
		for (auto j = vertSol.begin(); j != vertSol.end(); ++j) {
			std::cout << *j << ",";
		}
		std::cout << std::endl;
	}
}

void addSegments(std::vector<Vector2D> &points, ulong segmentAmount, std::vector<Segment<NT>> &segments, std::mt19937_64 &randomS) {
	using longPair = std::pair<unsigned long, unsigned long>;
	auto dice_randS = std::bind(std::uniform_int_distribution<unsigned long>(0, points.size() - 1), randomS);

	NT maxX = 0,maxY = 0;
	std::set<longPair> indexPairs;
	for (unsigned long j = 0; j < segmentAmount; ++j) {
		auto vecIndex1 = dice_randS();
		auto vecIndex2 = dice_randS();
		while(vecIndex1 == vecIndex2) {
			vecIndex2 = dice_randS();
		}
		//std::cout << "Add segment " << vecIndex1 << " " << vecIndex2 << ": " << points.size() - 1<< std::endl;
		longPair pair = longPair(vecIndex1,vecIndex2);
		if(indexPairs.find(pair) == indexPairs.end()) {
			if(maxX < points[vecIndex1].x || maxX < points[vecIndex2].x) {
				maxX = points[vecIndex1].x > points[vecIndex2].x ?  points[vecIndex1].x : points[vecIndex2].x;
			}
			if(maxY < points[vecIndex1].y || maxX < points[vecIndex2].y) {
				maxY = points[vecIndex1].y > points[vecIndex2].y ?  points[vecIndex1].y : points[vecIndex2].y;
			}
			longPair pair2 = longPair(vecIndex2,vecIndex1);
			indexPairs.insert(pair);
			indexPairs.insert(pair2);

			segments.push_back(Segment<NT>(points[vecIndex1], points[vecIndex2]));
		}
	}
}

TEST(TestFrameworkTests,EvolutionTest) {
	std::cout << std::endl;

	ulong seed = 2345243;
	ulong segmentAmount = 50;
	uint k = 25;

	TestRawPointsContainer conti;
	conti.importInstances("/Data/git/ba-hill/hip_instances/");
	//auto instances = conti.getTestInstances("von_koch-random-15-2");
	//auto instances = conti.getTestInstances("random-x-monotone-15-42");
	//auto instances = conti.getTestInstances("von_koch-random-20-3");
	auto vectorPoints = Cap2DGenerator::getRandomPoints(15,500,500,seed + 14865132);
	auto randPointsInstance = transformVector2DToTestInstance(vectorPoints);
	std::vector<TestRawPoints<NT>*> instances = {&randPointsInstance};
	NT offsetXmin = std::numeric_limits<double>::max();
	NT offsetYmin = std::numeric_limits<double>::max();
	NT offsetXmax = std::numeric_limits<double>::min();
	NT offsetYmax = std::numeric_limits<double>::min();
	Vector2D mostRightPoint;
	Vector2D mostLeftPoint;

	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << instances[0]->points[0][i] << " " << instances[0]->points[1][i] << std::endl;
		if (instances[0]->points[0][i] <= offsetXmin) {
			offsetXmin = instances[0]->points[0][i];
			mostLeftPoint = Vector2D(instances[0]->points[0][i],instances[0]->points[1][i]);
		}
		if (instances[0]->points[1][i] <= offsetYmin) {
			offsetYmin = instances[0]->points[1][i];
		}
		if (instances[0]->points[0][i] >= offsetXmax) {
			offsetXmax = instances[0]->points[0][i];
			mostRightPoint = Vector2D(instances[0]->points[0][i],instances[0]->points[1][i]);
		}
		if (instances[0]->points[1][i] >= offsetYmax) {
			offsetYmax = instances[0]->points[1][i];
		}
	}
	if(offsetXmin > 0)
		offsetXmin = 0;
	if(offsetYmin > 0)
		offsetYmin = 0;

	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
		instances[0]->points[0][i] = instances[0]->points[0][i] + CGAL::abs(offsetXmin);
		instances[0]->points[1][i] = instances[0]->points[1][i] + CGAL::abs(offsetYmin);
	}

	offsetXmin = std::numeric_limits<double>::max();
	offsetYmin = std::numeric_limits<double>::max();
	offsetXmax = std::numeric_limits<double>::min();
	offsetYmax = std::numeric_limits<double>::min();

	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << instances[0]->points[0][i] << " " << instances[0]->points[1][i] << std::endl;
		if (instances[0]->points[0][i] <= offsetXmin) {
			offsetXmin = instances[0]->points[0][i];
			mostLeftPoint = Vector2D(instances[0]->points[0][i],instances[0]->points[1][i]);
		}
		if (instances[0]->points[1][i] <= offsetYmin) {
			offsetYmin = instances[0]->points[1][i];
		}
		if (instances[0]->points[0][i] >= offsetXmax) {
			offsetXmax = instances[0]->points[0][i];
			mostRightPoint = Vector2D(instances[0]->points[0][i],instances[0]->points[1][i]);
		}
		if (instances[0]->points[1][i] >= offsetYmax) {
			offsetYmax = instances[0]->points[1][i];
		}
	}
	std::cout << "offsetXmin: " << offsetXmin << std::endl;
	std::cout << "offsetYmin: " << offsetYmin << std::endl;

	std::cout << "offsetXmax: " << offsetXmax << std::endl;
	std::cout << "offsetYmax: " << offsetYmax << std::endl;
	std::cout << "left point: " << mostLeftPoint << std::endl;
	std::cout << "right point: " << mostRightPoint << std::endl;


	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+1);

	auto points = transformTestInstanceToVector2D(instances[0]->points,instances[0]->points[0].size());
	std::vector<Segment<NT>> segments;

	addSegments(points,segmentAmount,segments,random);
	std::cout << "Segment amount 1.: " << segments.size() << std::endl;
	NT distanceX = offsetXmax - offsetXmin;

	NT distanceY = offsetYmax - offsetYmin;


	std::cout << "distance X: " << distanceX << " distance Y: " << distanceY << std::endl;
	for (uint i = 0; i < instances[0]->points[0].size(); ++i) {
		//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
		instances[0]->points[0][i] = instances[0]->points[0][i] + distanceX + 100;
		//instances[0]->points[1][i] = instances[0]->points[1][i] + distanceY;
	}
	points = transformTestInstanceToVector2D(instances[0]->points,instances[0]->points[0].size());
	addSegments(points,segmentAmount,segments,randomY);
	std::cout << "Segment amount 2.: " << segments.size() << std::endl;
	NT distanceLeftRightX = CGAL::abs(mostLeftPoint.x + distanceX + 100) - CGAL::abs(mostRightPoint.x);
	NT distanceLeftRightY = CGAL::abs(mostRightPoint.y - mostLeftPoint.y);

	std::cout << "distanceLR X: " << distanceLeftRightX << " distanceLR Y: " << distanceLeftRightY << std::endl;

	//build small segments between leftmost and rightmost vertex
	NT avgXdist = distanceLeftRightX / 25;
	NT avgYdist = distanceLeftRightY / 25;
	bool alternatingY = false;
	Vector2D travelVec = mostRightPoint;
	if(avgYdist == 0) {
		alternatingY = true;
		avgYdist = 5;
	}
	for (int j = 0; j < 49; ++j) {
		Vector2D vec1 = travelVec;
		if(j % 2 == 0) {
			if(alternatingY) {
				if(j % 4 == 0) {
					travelVec.y += avgYdist;
				} else {
					travelVec.y -= avgYdist;
				}
			} else {
				travelVec.y += avgYdist;
			}

		} else {
			travelVec.x += avgXdist;
		}
		segments.push_back(Segment<NT>(vec1,travelVec));
	}
	Vector2D vector2D(mostLeftPoint.x + distanceX + 100,mostLeftPoint.y);
	segments.push_back(Segment<NT>(travelVec,vector2D));

	std::cout << "instances: " << instances.size() << std::endl;

	//uint segmentPercentage = 40;
	//uint iterations = 1;

	Capturing2D capturing2D("TwoClusterInstance", segments,
							(uint) (CGAL::to_double(mostRightPoint.x) + CGAL::to_double(distanceX) + 110),
							(uint) (CGAL::to_double(offsetYmax) + 10), "Custom", 0);
	capturing2D.convertSegments2Graph();
	Metaheuristic::EvolveSettings evolveSettings;
	evolveSettings.startPopulation = 100;
	evolveSettings.useIteratedMutation = false;
	evolveSettings.maxIteration = 100;
	evolveSettings.populationSize = 500;
	Metaheuristic::EvolutionComputation evolutionComputation;
	evolutionComputation.setSettings(evolveSettings);
	//CapturingSolution capSol = evolutionComputation.solve(&capturing2D);
	capturing2D.setCplexOutput(true);
	capturing2D.setK(k);
	CapturingSolution capSol = capturing2D.solveKCapturing();
	CapturingSolution greedySol = capturing2D.solveGreedy();
	std::cout << "greedy: " << greedySol << std::endl;
	capturing2D.printGraph("testDoubleCluster",capSol);
	capturing2D.printGraph("testDoubleClusterGreedy",greedySol);
}*/