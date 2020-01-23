//
// Created by gharg on 21.08.17.
//

#include <gtest/gtest.h>
#include "../Capturing/Header/Capturing2D.h"
#include "../Import/Imports.h"
#include "../Generator/Header/Cap2DGenerator.h"
#include "../Solver/Metaheuristics/SimulatedAnnealing.h"
#include "../Solver/Metaheuristics/TabuSearch.h"
#include "../Solver/Wrapper/SolverWrapper.hpp"
#include "../Solver/Metaheuristics/EvolutionComputation.h"
#include "../MacroTestingFramework/MacroTestingInstance.hpp"
#include "../MacroTestingFramework/TestRawPointsContainer.h"
#include "../Generator/Wrapper/GeneratorWrapper.hpp"
#include "../Capturing/Neighborhoods/OuterSharedSegmentsVertices.h"
#include "../Solver/Solver/AllEdgesIPSolver.h"
#include "../Solver/Solver/ThesisIPSolver.h"

using namespace GeneratorCap2D;
namespace {
	/*class ApproxTests : public testing::Test {
	protected:
		Capturing2D capObj = Capturing2D(5);
		virtual void SetUp() {
			//Setup a SegmentGraph and and test and convert it
			//-k 5 -n 10 -m 20 -x 1000 -y 1000 -s 5
			capObj = Capturing2D(5);
			capObj.generateRandomPoints2D(10,20,1000,1000,5);
			capObj.convertSegments2Graph();
			auto greedySol = capObj.solveGreedy();
			capObj.setVertexSolution(greedySol);
		}

	};*/

	/*TEST_F(ApproxTests, GreedyInit) {
		std::cout << "test1" << std::endl;
	}
	TEST_F(ApproxTests, LongestSegment) {
		std::cout << "test1" << std::endl;
	}
	TEST_F(ApproxTests, GreedyLoop) {
		std::cout << "test1" << std::endl;
	}*/
	/*
	TEST(ApproxSoloTest, AugmentLength) {
		std::vector<Segment<NT>> segments(13);
		NT magnification = 10;
		segments[0] = Segment<NT>((NT)(0*magnification),(NT)(2*magnification),(NT)(15*magnification),(NT)(2*magnification));

		segments[1] = Segment<NT>(1*magnification,2*magnification,2*magnification,0*magnification);
		segments[2] = Segment<NT>(1*magnification,2*magnification,2*magnification,3*magnification);
		segments[3] = Segment<NT>(2*magnification,3*magnification,3*magnification,2*magnification);
		segments[4] = Segment<NT>(2*magnification,0*magnification,3*magnification,2*magnification);

		segments[5] = Segment<NT>(4*magnification,2*magnification,5*magnification,0*magnification);
		segments[6] = Segment<NT>(4*magnification,2*magnification,5*magnification,4*magnification);
		segments[7] = Segment<NT>(5*magnification,4*magnification,6*magnification,2*magnification);
		segments[8] = Segment<NT>(5*magnification,0*magnification,6*magnification,2*magnification);

		segments[9]  = Segment<NT>(7*magnification,2*magnification,8*magnification,1*magnification);
		segments[10] = Segment<NT>(7*magnification,2*magnification,8*magnification,3*magnification);
		segments[11] = Segment<NT>(8*magnification,3*magnification,9*magnification,2*magnification);
		segments[12] = Segment<NT>(8*magnification,1*magnification,9*magnification,2*magnification);

		Capturing2D graph = Capturing2D(4, segments, 0, 0);
		graph.setBounds((uint)(20*10),(uint)(8*10));
		graph.convertSegments2Graph();


		CapturingSolution exactSol = graph.solveKCapturing();
		CapturingSolution greedySol = graph.solveGreedy();

		std::string docName = "ApproxTest";
		CapturingSolution addDeleteTestSolution = exactSol;
		std::cout << "normal solution value: " << addDeleteTestSolution.getSolutionValue() << std::endl;
		std::cout << "vertexFamilyMapping: " << (*graph.getVertexInFamiliesMapping().begin()).second.size() << std::endl;
		addDeleteTestSolution.addVertex(0);
		addDeleteTestSolution.deleteVertex(1);
		std::cout << "with vertex 0 solution value: " << addDeleteTestSolution.getSolutionValue() << std::endl;
		auto &vertSol = addDeleteTestSolution.getVertexSolution();
		for (auto i = vertSol.begin(); i != vertSol.end(); ++i) {
			std::cout << "vertice: " << *i << std::endl;
		}
		std::cout << "finished looking."  << std::endl;

		std::string greedyName = docName;
		greedyName.append("Greedy");
		graph.printGraph(docName,exactSol);
		graph.setCplexPreSolution(greedySol);
		graph.printGraph(greedyName,greedySol);


		//EXPECT_EQ(exactSol.getVertexSolution(),greedySol.getVertexSolution());
	}*/
	/*
	TEST(QuadraticTest, QuadTest) {
		std::vector<Segment<NT>> segments(4);
		NT magnification = 10;
		segments[0] = Segment<NT>((NT)(0*magnification),(NT)(0*magnification),(NT)(1*magnification),(NT)(0*magnification));
		segments[1] = Segment<NT>(0*magnification,0*magnification,0*magnification,1*magnification);
		segments[2] = Segment<NT>(1*magnification,0*magnification,1*magnification,1*magnification);
		segments[3] = Segment<NT>(0*magnification,1*magnification,1*magnification,1*magnification);

		Capturing2D graph = Capturing2D("QuadTestInstance", segments, 0, 0, "Custom", 0);
		graph.setBounds((uint)(CGAL::to_double(1*magnification)),(uint)(CGAL::to_double(1*magnification)));
		graph.convertSegments2Graph();


		CplexSettings cplexSettings;
		cplexSettings.variableResultOutput = true;
		ThesisIPSolver ipSolver;
		ThesisIPSolver lpSolver;
		ipSolver.setSettings(cplexSettings);
		cplexSettings.relaxation = true;
		lpSolver.setSettings(cplexSettings);
		AllEdgesIPSolver lpSolver2;
		lpSolver2.setSettings(cplexSettings);
		CapturingSolution ipSol = ipSolver.solve(&graph,2);
		CapturingSolution lpSol = lpSolver.solve(&graph,2);
		CapturingSolution lpSol2 = lpSolver2.solve(&graph,2);
		std::string docName = "QuatTest";
		std::string lpName = docName;
		lpName.append("LP");
		graph.printGraph(docName,ipSol);
		graph.printGraph(lpName,lpSol);
		auto lpName2 = lpName;
		lpName2.append("2");
		graph.printGraph(lpName2,lpSol2);

		//EXPECT_EQ(exactSol.getVertexSolution(),greedySol.getVertexSolution());
	}

	TEST(LPvsIPTests,WorkingRelaxationTest) {
		GeneratorCap2D::TwoAxisCapturingNoOverlap_Generator gen;
		GeneratorCap2D::GeneratorSettings genSettings = gen.getSettings();

		genSettings.seed = 5;
		genSettings.yBound = 1000;
		genSettings.xBound = 1000;
		genSettings.horAmount = 50;
		genSettings.vertAmount = 50;
		genSettings.m = 250;
		gen.setSettings(genSettings);
		std::vector<Capturing2D> instances;
		std::vector<std::string> instanceNames;
		gen.generateInstances(&instances,10,genSettings.seed);
		std::cout << "instances generated" << std::endl;
		ThesisIPSolver IPSolver;
		ThesisIPSolver LPSolver;

		auto lPSettings = LPSolver.getSettings();
		lPSettings.relaxation = true;
		LPSolver.setSettings(lPSettings);
		for (auto instance : instances) {
			auto ipSolution = IPSolver.solve(&instance,5);
			auto lpSolution = LPSolver.solve(&instance,5);
			
			EXPECT_EQ(ipSolution.getSolutionValue(),lpSolution.getSolutionValue());
		}
	}


	//Deprecated Test
	/*
	TEST(ApproxVsCplexTest,Test10RandomInstances) {
		std::string docName = "BatchTest";
		for (int i = 0; i < 10; ++i) {
			Capturing2D capGraph = Capturing2D(docName);
			std::fstream stream;
			capGraph.setCplexOutput(0);
			std::cout << "Testing n:" << 100 + i*10 << " m: " << 200+i*25 << " s: " << 100+i*10;
			capGraph.generateRandomPoints2D(100 + i*10,100+i*15,1000,1000,100+i*10);

			capGraph.convertSegments2Graph();
			double size = (double)capGraph.getGraph()->getSize();
			double density =  ((size * (size-1))/2) / (double)capGraph.getGraph()->getEdges().size();
			std::cout << "density: " << density << std::endl;
			
			auto greedySol = capGraph.solveGreedy();
			CapturingSolution exactSol = capGraph.solveKCapturing();
			auto greedyVal = greedySol.getSolutionValue();
			auto exactVal = exactSol.getSolutionValue();
			auto abs =  exactVal/2;
			std::cout << "Greedy solution value: " << greedyVal << std::endl << "Exact solution value: " << exactVal << std::endl;
			std::string internalDocName = docName;
			internalDocName.append(std::to_string(i));
			std::string internalDocNameGreedy = internalDocName;
			internalDocNameGreedy.append("Greedy");
			capGraph.setCplexPreSolution(exactSol);
			capGraph.printGraph(internalDocName);
			capGraph.setCplexPreSolution(greedySol);
			capGraph.printGraph(internalDocNameGreedy);
			EXPECT_NEAR(greedyVal,exactVal,abs);
		}
	}*/
	/*
	TEST(GraphTests,svgImportTest) {
		std::ifstream myfile;
		myfile.open("importExample.svg");
		Capturing2D graph = Imports::importCapturingSVG(myfile, "test");
		graph.convertSegments2Graph();
		EXPECT_EQ(5,graph.getGraph()->getSize());
		std::string docname("importExampleOutput");
		graph.printGraph(docname);

	}

	TEST(GraphTests,csvImportTest) {
		std::ifstream myfile;
		myfile.open("att48.csv");
		auto points = Imports::importCSV(myfile, "\n", "\t");
		Capturing2D graph = Cap2DGenerator::generateRandomAllNodesInstance(points, 200, 30, "test");
		Capturing2D graph2 = Cap2DGenerator::generateRandomSharedEndPointsInstance(points, 200, 30, "test");
		graph.convertSegments2Graph();
		graph2.convertSegments2Graph();
		std::string docName("att48");
		std::string docName2("att48_2");
		//graph.setBounds(8000, 8000);
		//graph.setK(5);
		std::cout << "amount of vertices prob: " << graph.getGraph()->getSize() << std::endl;
		std::cout << "amount of vertices rand: " << graph2.getGraph()->getSize() << std::endl;
		std::cout << "amount of segments prob: " << graph.getVertexFamilies()->size() << std::endl;
		std::cout << "amount of segments rand: " << graph2.getVertexFamilies()->size() << std::endl;
		auto greedySol = graph.solveGreedy();
		graph.printGraph(docName,greedySol);

		auto greedySol2 = graph2.solveGreedy();
		graph2.printGraph(docName2,greedySol2);
		auto exactSol = graph.solveKCapturing();
		auto exactSol2 = graph2.solveKCapturing();
		//std::cout << "exactSol prob: " << exactSol.getSolutionValue() << " exactSol rand: " << exactSol2.getSolutionValue() << std::endl;

		//EXPECT_EQ((48*47)/2,graph.getVertexFamilies()->size());

	}

	TEST(GraphTests,solutionNeighbourTest) {
		using namespace Metaheuristic;
		ulong seed = 5;
		ulong k = 10;
		Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
				Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed), 20, seed + 1, "Test");
		capGraph.convertSegments2Graph();
		auto greedySol = capGraph.solveGreedy();
		std::cout << "greedy sol: " << greedySol.getSolutionValue() << "amount vertice: " << greedySol.getVertexSolution().size() << std::endl;
		auto iteratedSol = greedySol.iteratedLocalSearch(false);
		std::cout << "iterated sol: " << iteratedSol.getSolutionValue() << std::endl;

		SimulatedAnnealing simulatedAnnealing;
		simulatedAnnealing.setTemperatureFunc(temperatureSlowDown05NoReheatFunction);
		simulatedAnnealing.setProbabilityFunc(bolzmannFunction);
		auto simAnnealSol = simulatedAnnealing.solve(&capGraph, 0, nullptr);
		std::cout << "simulated annealing sol: " << simAnnealSol.getSolutionValue() << std::endl;
		std::string docname4("SimulatedAnnealing");
		capGraph.printGraph(docname4,simAnnealSol);

		TabuSearch tabuSearch;
		auto tabuSol = tabuSearch.solve(capGraph);
		std::cout << "Tabu search sol: " << tabuSol.getSolutionValue() << std::endl;

		std::string docname("iteratedLocalSearch");
		std::string docname2("iteratedLocalSearchGreedy");
		std::string docname3("iteratedLocalSearchExact");
		capGraph.printGraph(docname,iteratedSol);
		capGraph.printGraph(docname2,greedySol);
		capGraph.setCplexOutput(false);
		auto exactSol = capGraph.solveKCapturing();
		std::cout << "exact sol: " << exactSol.getSolutionValue() << std::endl;
		capGraph.printGraph(docname3,exactSol);
	}

	TEST(MetaheurTests,bolzmannTests) {
		using namespace Metaheuristic;
		std::cout << std::endl;
		//SimulatedAnnealing simulatedAnnealing;
		//simulatedAnnealing.setTemperatureFunc(temperatureSlowDown05Reheat50Function);
		//simulatedAnnealing.setProbabilityFunc(bolzmannFunction);
		double T = 50;
		double curr = 100;
		double inner = 50;
		for (int j = 1; j <= 501; ++j) {

			std::cout << "iteration: " << j << " Temperature: " << T << " Probability: " << (uint)(bolzmannFunction(curr,inner,T)*100) << std::endl;
			T = temperatureSlowDown05Reheat50Function(T, j);
		}


	}

	TEST(GraphTests,solutionLocalGlobalNeighbourIteratedTest) {
		//using namespace Metaheuristic;
		ulong seed = 5;
		ulong k = 10;
		std::cout << std::endl;
		for (int j = 0; j < 10; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
					Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed), 30, seed + 1 + j, "Test");
			capGraph.convertSegments2Graph();
			Capturing2D_Random_solution randSolGen;
			auto randSol = randSolGen.solve(&capGraph,k);

			std::cout << "Random sol: " << randSol.getSolutionValue() << " Amount vertice: "
					  << randSol.getVertexSolution().size() << std::endl;
			auto greedySol = capGraph.solveGreedy();
			std::cout << "Greedy sol: " << greedySol.getSolutionValue() << " Amount vertice: "
					  << greedySol.getVertexSolution().size() << std::endl;
			auto begin_time1 = std::chrono::high_resolution_clock::now();


			auto iteratedSol = randSol.iteratedLocalSearch(false);
			auto end_time1 = std::chrono::high_resolution_clock::now();

			auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time1 - begin_time1).count();
			std::cout << "iterated global sol: " << iteratedSol.getSolutionValue() << " time: " << nanoseconds1
					  << std::endl;

			auto begin_time2 = std::chrono::high_resolution_clock::now();
			auto iteratedLocalSol = randSol.iteratedLocalSearch(true);
			auto end_time2 = std::chrono::high_resolution_clock::now();

			auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time2 - begin_time2).count();
			std::cout << "iterated local  sol: " << iteratedSol.getSolutionValue() << " time: " << nanoseconds2
					  << std::endl;
			EXPECT_GE(nanoseconds1, nanoseconds2);
			EXPECT_NEAR(iteratedSol.getSolutionValue(),iteratedLocalSol.getSolutionValue(),iteratedLocalSol.getSolutionValue()*0.05);
		}
	}

	TEST(GraphTests,solutionLocalGlobalNeighbourSimulatedTest) {
		//using namespace Metaheuristic;
		ulong seed = 5;
		ulong k = 10;
		std::cout << std::endl;
		for (int j = 0; j < 10; ++j) {
			std::cout << "Start iteration: " << j << std::endl;
			Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
					Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed), 30, seed + 1 + j, "Test");
			capGraph.convertSegments2Graph();
			Metaheuristic::SimulatedAnnealing simulatedAnnealing;
			simulatedAnnealing.setTemperatureFunc(Metaheuristic::temperatureSlowDown05NoReheatFunction);
			Capturing2D_Random_solution randSolGen;
			auto randSol = randSolGen.solve(&capGraph,k);

			std::cout << "Random sol: " << randSol.getSolutionValue() << " Amount vertice: "
					  << randSol.getVertexSolution().size() << std::endl;
			auto greedySol = capGraph.solveGreedy();
			std::cout << "Greedy sol: " << greedySol.getSolutionValue() << " Amount vertice: "
					  << greedySol.getVertexSolution().size() << std::endl;

			simulatedAnnealing.setUseLocal(false);
			auto begin_time1 = std::chrono::high_resolution_clock::now();
			auto simulSol = simulatedAnnealing.solve(&capGraph, 0, &greedySol);
			auto end_time1 = std::chrono::high_resolution_clock::now();

			auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time1 - begin_time1).count();
			std::cout << "Simulated annealing global sol: " << simulSol.getSolutionValue() << " time: " << nanoseconds1
					  << std::endl;

			simulatedAnnealing.setUseLocal(true);
			auto begin_time2 = std::chrono::high_resolution_clock::now();
			auto simulLocalSol = simulatedAnnealing.solve(&capGraph, 0, &greedySol);
			auto end_time2 = std::chrono::high_resolution_clock::now();

			auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time2 - begin_time2).count();
			std::cout << "Simulated annealing local  sol: " << simulLocalSol.getSolutionValue() << " time: " << nanoseconds2
					  << std::endl;
			//EXPECT_GE(nanoseconds1, nanoseconds2);
			EXPECT_GE(simulLocalSol.getSolutionValue(),simulSol.getSolutionValue());
			std::cout << "End iteration: " << j << std::endl;
		}
	}

	TEST(GraphTests,solutionRandomStartGreedyTest) {
		//using namespace Metaheuristic;
		ulong seed = 5;
		ulong k = 10;
		std::cout << std::endl;
		std::mt19937 random(seed + 45567576);
		for (uint j = 0; j < 10; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
					Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed + j), 30, seed + 1 + j, "Test");
			capGraph.convertSegments2Graph();

			std::uniform_int_distribution<ulong> distribution(0,capGraph.getVertexFamilies()->size() - 1);
			ulong randSegIndex = distribution(random);
			std::cout << "Start solving: " << std::endl;
			auto begin_time1 = std::chrono::high_resolution_clock::now();
			auto randSol1 = capGraph.solveGreedy(randSegIndex);
			auto end_time1 = std::chrono::high_resolution_clock::now();

			auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time1 - begin_time1).count();
			std::cout << "greedy sol: " << randSol1.getSolutionValue() << " time: " << nanoseconds1 << " segIndex: " << randSegIndex
					  << std::endl;

			auto begin_time2 = std::chrono::high_resolution_clock::now();

			auto greedySol = capGraph.solveGreedy();
			auto end_time2 = std::chrono::high_resolution_clock::now();

			auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(
					end_time2 - begin_time2).count();
			std::cout << "Greedy standard sol: " << greedySol.getSolutionValue() << " time: " << nanoseconds2
					  << std::endl;
			//EXPECT_GE(nanoseconds1, nanoseconds2);
			EXPECT_GE(greedySol.getSolutionValue(),randSol1.getSolutionValue());
		}
	}

	TEST(IntersectionTest,IntersectionError) {
		std::cout << std::endl;
		Segment<NT> seg1(-168.82704664021730423,609.306720085442066193,420.372303575277328491,837.395691312849521637);
		Segment<NT> seg2(-168.82704664021730423,609.306720085442066193,-791.63836408406496048,-844.701102934777736664);
		Segment<NT> seg3(-168.82704664021730423,609.306720085442066193,-260.0156441330909729,-850.601775571703910828);
		auto vec = GeoUtil::getIntersectionPoint(seg1,seg2);
		auto vec2 = GeoUtil::getIntersectionPoint(seg1,seg3);
		auto vec3 = GeoUtil::getIntersectionPoint(seg2,seg3);
		std::cout << std::fixed << std::setprecision(20) << seg1.GetStartVector() << std::endl;
		std::cout << std::fixed << std::setprecision(20) << vec.first << std::endl;
		std::cout << std::fixed << std::setprecision(20) << vec2.first << std::endl;
		std::cout << std::fixed << std::setprecision(20) << vec3.first << std::endl;
	}

	TEST(GraphTest,EvolutioComputation) {
		std::cout << std::endl;
		ulong seed = 5;
		ulong k = 10;
		for (uint j = 0; j < 10; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generateRandomAllNodesInstance(
					Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed + j), 30, seed + 1 + j, "Test");
			capGraph.convertSegments2Graph();

			Metaheuristic::EvolutionComputation evolutionComputation;
			Metaheuristic::EvolveSettings settings;
			settings.maxIteration = 1000;
			settings.startPopulation = 50;
			settings.populationSize = 500;
			settings.useIteratedMutation = false;
			evolutionComputation.setSettings(settings);
			CapturingSolution evoSol = evolutionComputation.solve(&capGraph, 0);

			std::cout << "Evolution sol: " << evoSol.getSolutionValue()
					  << std::endl;
			capGraph.setCplexPreSolution(evoSol);
			CapturingSolution exactSol = capGraph.solveKCapturing();

			std::cout << "exact sol: " << exactSol.getSolutionValue()
					  << std::endl;
		}
	}

	TEST(GraphTest,SimulatedAnnealingComputation_Test) {
		std::cout << std::endl;
		ulong seed = 5;
		ulong k = 10;
		for (uint j = 0; j < 10; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
					Cap2DGenerator::getRandomPoints(50, 1000, 1000, seed + j), 15, seed + 1 + j, "Test");
			capGraph.convertSegments2Graph();

			OuterSharedSegmentsVertices ossvNeighborhood;
			Metaheuristic::SimulatedAnnealing simulatedAnnealing;
			Metaheuristic::SASettings settings;
			settings.maxIteration = 1000;
			settings.useLocal = true;
			settings.maxThreads = 1;
			simulatedAnnealing.setSettings(settings);
			CapturingSolution saSol = simulatedAnnealing.solve(&capGraph, 0, 0);

			std::cout << "Sim sol: " << saSol.getSolutionValue()
					  << std::endl;
			settings.neighborhood = &ossvNeighborhood;
			simulatedAnnealing.setSettings(settings);
			CapturingSolution saNeighborhoodSol = simulatedAnnealing.solve(&capGraph, 0, 0);

			std::cout << "Sim sol new neighborhood: " << saNeighborhoodSol.getSolutionValue()
					  << std::endl;
		}
	}
	TEST(GraphTest,IteratedSearchComputation_Test) {
		std::cout << std::endl;
		ulong seed = 5;
		ulong k = 10;
		for (uint j = 0; j < 10; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generateProbabilisticInstance(
					Cap2DGenerator::getRandomPoints(50 + (5 * j), 1000, 1000, seed + j), 15, seed + seed + j, "Test");
			capGraph.convertSegments2Graph();

			OuterSharedSegmentsVertices ossvNeighborhood;
			CapturingSolution saSol = capGraph.solveGreedy();
			auto begin_time = std::chrono::high_resolution_clock::now();
			CapturingSolution localSaSol = saSol.iteratedLocalSearch(true);
			auto end_time = std::chrono::high_resolution_clock::now();
			auto milliseconds1 = std::chrono::duration_cast<std::chrono::milliseconds>(
					end_time - begin_time).count();

			begin_time = std::chrono::high_resolution_clock::now();
			CapturingSolution saNeighborhoodSol = saSol.iteratedLocalSearch(&ossvNeighborhood);
			end_time = std::chrono::high_resolution_clock::now();
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
					end_time - begin_time).count();
			std::cout << "Sim sol: " << localSaSol.getSolutionValue() << " Time: " << milliseconds1
					  << std::endl;
			std::cout << "Sim sol new neighborhood: " << saNeighborhoodSol.getSolutionValue() << " Time: " << milliseconds
					  << std::endl;
		}
	}

	TEST(GraphTest,FirstGraphTest) {
		/*
		NT magnifier = 20;
		Segment<NT> seg1(0,0,4,4);
		Segment<NT> seg2(3,0,3,4);
		Segment<NT> seg3(1,2,6,1);
		Segment<NT> seg4(4,4,6,1);
		std::vector<Segment<NT>> segments = {seg1*magnifier,seg2*magnifier,seg3*magnifier,seg4*magnifier};
		Capturing2D capturing2D("",segments,8*20,8*20);
		CapturingSolution cplexSol = capturing2D.solveKCapturing();
		capturing2D.printGraph("EasyGraph",cplexSol);

		 */

	/*
	}

	TEST(GraphTest,GraphTest_HorizontalVerticalGraph_generation_Test) {
		for (ulong j = 10; j < 20; ++j) {

			Capturing2D capGraph = Cap2DGenerator::generate2DAxisAlignedInstance(300+j*50,30+j*5,30+j*5,1000,1000,165+j);

			capGraph.setK(10);
			CapturingSolution sol = capGraph.solveKCapturing();
			AllEdgesIPSolver newIpSolver;
			CapturingSolution newSol = newIpSolver.solve(&capGraph, 0);

			capGraph.printGraph("HorVertGraph_" + std::to_string(j),sol);
			capGraph.printGraph("HorVertGraph_otherIP_" + std::to_string(j),newSol);
			ASSERT_EQ(sol.getSolutionValue(),newSol.getSolutionValue());
		}
	}

	TEST(GraphTests,GeneratorDrawTest) {
		std::cout << std::endl;

		ulong seed = 2345243;
		ulong segmentAmount = 30;
		//uint k = 25;
		using namespace MacroTesting;
		TestRawPointsContainer conti;
		conti.importInstances("/Data/git/ba-hill/hip_instances/");
		auto instances = conti.getTestInstances("berlin52");
		std::cout << "instances size: " << instances.size() << std::endl;
		RandomSharedEndPointsInstanceGenerator randomSharedEndPointsInstanceGenerator;
		RandomAllNodesInstanceGenerator randomAllNodesInstanceGenerator;
		ProbabilisticInstanceGenerator probabilisticInstanceGenerator;

		randomSharedEndPointsInstanceGenerator.setIsRelativeSegmentAmount(true);
		randomAllNodesInstanceGenerator.setIsRelativeSegmentAmount(true);

		for (uint j = 5; j <= segmentAmount; j += 5) {

			randomAllNodesInstanceGenerator.setSegmentAmount(1);
			randomSharedEndPointsInstanceGenerator.setSegmentAmount(1);
			probabilisticInstanceGenerator.setSegmentAmount(1);

			auto rAInstance = randomAllNodesInstanceGenerator.generateInstance(instances[0]->points,
																			   instances[0]->amount, j, seed,
																			   "Test");
			//auto rSInstance = randomSharedEndPointsInstanceGenerator.generateInstance(instances[0]->points,instances[0]->amount,j,seed);
			//auto pSInstance = probabilisticInstanceGenerator.generateInstance(instances[0]->points,instances[0]->amount,j,seed);
			std::cout << "instances generated" << std::endl;
			rAInstance.printGraph(std::string("randomAllS").append(std::to_string(j)));
			//rSInstance.printGraph(std::string("randomSharedS").append(std::to_string(j)));
			//pSInstance.printGraph(std::string("probabilisticSharedS").append(std::to_string(j)));
		}

	}
	TEST(GraphTests,svgImportExportedTest) {
		std::ifstream myfile;
		std::cout << std::endl;
		boost::filesystem::path filePath("/home/gharg/results/result606/von_koch-random-55-3_iteration_0.svg");
		std::cout << filePath<< std::endl;



		if (boost::filesystem::exists(filePath)) {
			std::cout << "file/directory exists" << std::endl;
			if (boost::filesystem::is_directory(filePath)) {
				for (boost::filesystem::directory_entry &it : boost::filesystem::directory_iterator(filePath)) {
					myfile.open(it.path().c_str());
					Capturing2D graph = Imports::importCapturingSVG(myfile, "Test");
					graph.setK(25);
					std::cout << "vertices: " << graph.getGraph()->getSize() << std::endl;
					CapturingSolution IpSol = graph.solveKCapturing();
					CapturingSolution IteratedSol = graph.solveGreedy().iteratedLocalSearch(true);
				}

			} else if (boost::filesystem::is_regular_file(filePath)) {
				std::cout << "is regular file" << std::endl;
				myfile.open(filePath.c_str());
				Capturing2D graph = Imports::importCapturingSVG(myfile, "Test");
				graph.setK(25);
				std::cout << "vertices: " << graph.getGraph()->getSize() << std::endl;
				CapturingSolution greedySol = graph.solveGreedy();
				std::cout << "Greedy sol: " << greedySol.getSolutionValue() << std::endl;
				CapturingSolution IteratedSol = greedySol.iteratedLocalSearch(true);
				//graph.setCplexPreSolution(IteratedSol);
				CapturingSolution IpSol = graph.solveKCapturing();
				IpSol.computeSolution(false,true);
				std::cout << IpSol << std::endl;
				//std::cout << IteratedSol << std::endl;
			}
		}





	}

	TEST(GraphTest,OverlappintSegments) {
		/*
		NT magnifier = 20;
		Segment<NT> seg1(0,1,10,1);
		Segment<NT> seg2(2,1,5,1);
		Segment<NT> seg3(2,1,8,1);
		Segment<NT> seg4(2,1,3,1);
		Segment<NT> seg5(2,1,5,1);
		Segment<NT> seg6(1,1,5,1);
		Segment<NT> seg7(1,1,3,1);
		std::vector<Segment<NT>> segments = {seg1*magnifier,seg2*magnifier,seg3*magnifier,seg4*magnifier
				,seg5*magnifier,seg6*magnifier,seg7*magnifier};
		for (int j = 0; j < 10; ++j) {
			segments.push_back(Segment<NT>((j+0.5),0,j+0.5,2)*magnifier);
		}
		Capturing2D capturing2D(4,segments,11*20,11*20);
		CapturingSolution cplexSol = capturing2D.solveKCapturing();
		std::cout << "cplex: " << cplexSol << std::endl;
		cplexSol.computeSolution(false,true);
		std::cout << "computed: " << cplexSol << std::endl;
		capturing2D.printGraph("OverlappingGraph",cplexSol);
		 */
	//}
}

