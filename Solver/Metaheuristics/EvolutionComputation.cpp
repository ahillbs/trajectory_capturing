//
// Created by gharg on 22.10.17.
//

#include <thread>
#include <future>
#include "EvolutionComputation.h"
#include "../Solver/GreedySolver.h"


CapturingSolution Metaheuristic::EvolutionComputation::solve(Capturing2D *problem, ulong maxK) {
    problem->convertSegments2Graph();
	auto begin_time = std::chrono::high_resolution_clock::now();

	Solutions population;
	std::cout << "Create population. Amount: " << settings.startPopulation << std::endl;
	generatePopulation(&population, *problem, maxK);

	std::cout << "Select first population" << std::endl;
	select(&population, nullptr);

	if(settings.useIteratedMutation) {
		//std::cout << "Mutate population" << std::endl;
		mutateIterated(&population);
	}


	ulong iteration = 0;
	ulong noChangedBase = 0;
	uint seconds = (uint)std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::_V2::system_clock::now() - begin_time).count();

	while(settings.maxTime > seconds && settings.maxIteration > iteration && noChangedBase < settings.maxNoChangedBase) {
		std::cout << "Start new iteration " << iteration << ". ";
		std::cout << "Best solution: " << getBestSolution(population).getSolutionValue() << std::endl;
		Solutions nextGeneration;
		//std::cout << "Recombine population" << std::endl;
		recombination(population,&nextGeneration);
		//std::cout << "Mutate population" << std::endl;
		if(settings.useIteratedMutation) {
			mutateIterated(&nextGeneration);
		} else {
			mutateLocalRandom(&nextGeneration, problem);
		}
		//std::cout << "Select population" << std::endl;
		if (!select(&population,&nextGeneration)) {
			++noChangedBase;
		}

		++iteration;
		seconds = (uint)std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::_V2::system_clock::now() - begin_time).count();
	}
	return settings.maxTime > seconds ? getBestSolution(population)
									  : getBestSolution(population).iteratedLocalSearch(true);
}

bool Metaheuristic::EvolutionComputation::select(Solutions *population, Solutions *secondGen) {
	std::map<double,std::pair<bool,ulong>> prioList;
	Solutions endSolutions;
	bool changedBase = false;
	for (uint j = 0; j < population->size(); ++j) {
		prioList.insert(std::make_pair((*population)[j].getSolutionValue(), std::make_pair(0, j)));
	}

	if(secondGen != nullptr) {
		for (uint j = 0; j < secondGen->size(); ++j) {
			prioList.insert(std::make_pair((*secondGen)[j].getSolutionValue(),std::make_pair(1,j)));
		}
	}

	auto it = prioList.rbegin();
	for (uint k = 0; k < settings.populationSize && it != prioList.rend(); ++k) {

		if(!it->second.first) {
			endSolutions.push_back((*population)[it->second.second]);
		} else {
			if(secondGen != nullptr) {
				//std::cout << "chose new gen solution " << (*secondGen)[it->second.second].getSolutionValue() << std::endl;
				changedBase = true;
				endSolutions.push_back((*secondGen)[it->second.second]);
			}
		}
		++it;
	}
	*population = endSolutions;
	return changedBase;
}

void innerMutate(Solutions *population,ulong begin,ulong end) {
	for (ulong j = begin; j < end; ++j) {
		(*population)[j] = (*population)[j].iteratedLocalSearch(true);
	}
}

void Metaheuristic::EvolutionComputation::mutateIterated(Solutions *population) {

	unsigned threadsSupported = std::thread::hardware_concurrency();

	if(threadsSupported > 1) {
		std::vector<ulong> counter(threadsSupported);
		for (uint l = 0; l < population->size(); ++l) {
			++counter[l % threadsSupported];
		}
		{
			std::vector<std::future<void>> threadSolutions;
			ulong allCount = 0;
			//std::cout << "Start Evo mutator with " << threadsSupported << " threads" << std::endl;
			for (uint j = 0; j < threadsSupported; ++j) {
				threadSolutions.push_back(
						std::async(std::launch::async,&innerMutate, population, allCount, allCount + counter[j]));
				allCount += counter[j];
			}
		}
	} else {
		innerMutate(population,0,population->size());
	}

}

std::pair<ulong, ulong>
getGenotypePair(Solutions &population, ulong seed) {
	std::mt19937_64 random(seed + 654984);
	std::pair<ulong,ulong> indexPair = std::make_pair(ulong()-1,ulong()-1);
	double allValues = 0;
	double worstValue = std::numeric_limits<double>::max();
	for (auto sol : population) {
		allValues  += sol.getSolutionValue();
		if(worstValue > sol.getSolutionValue())
		{
			worstValue = sol.getSolutionValue();
		}
	}
	allValues = allValues - population.size()*worstValue;
	std::uniform_real_distribution<double> realDistribution(0,allValues);
	double val1 = realDistribution(random);
	double val2 = realDistribution(random);
	//std::cout << "values: " << val1 << " " << val2 << " worst value: " << worstValue << " all values: " << allValues << std::endl;
	allValues = 0;
	for (ulong j = 0; j < population.size(); ++j) {
		allValues  += (population[j].getSolutionValue() - worstValue);
		if(val1 < allValues && indexPair.first > j) {
			indexPair.first = j;
		}
		if(val2 < allValues && indexPair.second > j) {
			indexPair.second = j;
		}
	}
	return indexPair;
}

void innerRecombination(Solutions &population, Solutions *nextGeneration, ulong seed, ulong populationSize) {
	std::mt19937_64 random(seed + 6549816);
	CapturingGraph *capPtr = population.begin()->getGraph();
	std::uniform_int_distribution<ulong> parent(0, 1);
	std::uniform_int_distribution<ulong> vertexIndex;
	for (uint k = 0; k < populationSize; ++k) {
		std::pair<ulong, ulong> indexPair = getGenotypePair(population, seed + k);
		//std::cout << "index pair: " << indexPair.first << " " << indexPair.second << std::endl;
		std::set<Vertex> newSolution;
		std::set<Vertex> parentSolutions[] = {population[indexPair.first].getVertexSolution(),
											  population[indexPair.second].getVertexSolution()};
		ulong iterations = population[indexPair.first].getVertexSolution().size();
		for (uint j = 0; newSolution.size() < iterations; ++j) {
			ulong parentIndex = parent(random);
			ulong maxSize = parentSolutions[parentIndex].size()-1;
			std::uniform_int_distribution<ulong>::param_type vertexIndexParam(0, maxSize);
			ulong vIndex = vertexIndex(random,vertexIndexParam);
			auto it = parentSolutions[parentIndex].begin();
			for (uint l = 0; l < vIndex; ++l) { ++it; }
			newSolution.insert(*it);
			parentSolutions[parentIndex].erase(*it);
		}
		CapturingSolution sol(newSolution,capPtr);
		//std::cout << "new gen sol val: " << sol.getSolutionValue() << std::endl;
		nextGeneration->push_back(sol);
	}
}

void Metaheuristic::EvolutionComputation::recombination(Solutions &population, Solutions *nextGeneration) {
	ulong iterations = settings.populationSize;
	unsigned threadsSupported = std::thread::hardware_concurrency();

	if(threadsSupported > 1) {
		std::vector<std::vector<CapturingSolution>> solutions(threadsSupported);
		std::vector<ulong> counter(threadsSupported);
		for (uint l = 0; l < iterations; ++l) {
			++counter[l % threadsSupported];
		}
		{
			std::vector<std::future<void>> threadSolutions;
			//std::cout << "Start Evo generator with " << threadsSupported << " threads" << std::endl;
			for (uint j = 0; j < threadsSupported; ++j) {
				threadSolutions.push_back(
						std::async(std::launch::async,&innerRecombination, std::ref(population), &(solutions[j]), settings.seed + j, counter[j]));
			}
		}
		for (uint j = 0; j < solutions.size(); ++j) {
			for (uint k = 0; k < solutions[j].size(); ++k) {
				nextGeneration->push_back(solutions[j][k]);
			}
		}
	} else {
		innerRecombination(population,nextGeneration, settings.seed, settings.populationSize);
	}
}

void innerGeneratePopulation(Solutions *population, Capturing2D &problem, ulong seed, ulong iterations, ulong maxSegments,
							 ulong maxK) {
	std::mt19937_64 random(seed + 8486135);

	std::uniform_int_distribution<ulong> segIndexRand(0,maxSegments-1);

	std::cout << "Start population: " << iterations << std::endl;
	for (uint j = 0; j < iterations; ++j) {
		//std::cout << "index: " << j << std::endl;
		ulong segIndex = segIndexRand(random);
		//std::cout << "Seed: " << seed << " segIndex: " << segIndex << std::endl;
		GreedySolver solver;
		population->push_back(solver.solve(&problem,maxK,segIndex));
	}
}

void Metaheuristic::EvolutionComputation::generatePopulation(std::vector<CapturingSolution> *population, Capturing2D &problem, ulong maxK) {
	ulong maxSegments = problem.getVertexFamilies()->size();
	ulong iterations = settings.startPopulation;
	unsigned threadsSupported = std::thread::hardware_concurrency();

	if(threadsSupported > 1) {
		std::vector<std::vector<CapturingSolution>> solutions(threadsSupported);
		std::vector<ulong> counter(threadsSupported);
		for (uint l = 0; l < iterations; ++l) {
			++counter[l % threadsSupported];
		}
		{
			std::vector<std::future<void>> threadSolutions;
			//std::cout << "Start Evo generator with " << threadsSupported << " threads" << std::endl;
			for (uint j = 0; j < threadsSupported; ++j) {
				threadSolutions.push_back(
						std::async(std::launch::async, &innerGeneratePopulation, &(solutions[j]),std::ref(problem), settings.seed + j, counter[j],maxSegments, maxK));
			}
		}
		for (uint j = 0; j < solutions.size(); ++j) {
			for (uint k = 0; k < solutions[j].size(); ++k) {
				population->push_back(solutions[j][k]);
			}
		}
	} else {
		innerGeneratePopulation(population, problem, settings.seed, iterations, maxSegments, maxK);
	}


}

CapturingSolution
Metaheuristic::EvolutionComputation::getBestSolution(Solutions &population) {
	double bestSolValue = 0;
	ulong bestIndex = 0;
	for (ulong j = 0; j < population.size(); ++j) {
		if (population[j].getSolutionValue() > bestSolValue) {
			population[j].computeSolution(false,true);
			bestSolValue = population[j].getSolutionValue();
			bestIndex = j;
		}
	}
	return population[bestIndex];
}



const Metaheuristic::EvolveSettings &Metaheuristic::EvolutionComputation::getSettings() const {
	return settings;
}

void Metaheuristic::EvolutionComputation::setSettings(const Metaheuristic::EvolveSettings &settings) {
	EvolutionComputation::settings = settings;
}

void innerMutateLocalRandom(Solutions *population, ulong begin, ulong end, Metaheuristic::SASettings saSetting,
							Capturing2D *problem) {
	for (ulong j = begin; j < end; ++j) {
		Metaheuristic::SimulatedAnnealing simulatedAnnealing;
		simulatedAnnealing.setSettings(saSetting);

		(*population)[j] = simulatedAnnealing.solve(problem, 0, &(*population)[j]);
	}
}

void Metaheuristic::EvolutionComputation::mutateLocalRandom(Solutions *population, Capturing2D *problem) {
	unsigned threadsSupported = std::thread::hardware_concurrency();

	if(threadsSupported > 1) {
		std::vector<ulong> counter(threadsSupported);
		for (uint l = 0; l < population->size(); ++l) {
			++counter[l % threadsSupported];
		}
		{
			std::vector<std::future<void>> threadSolutions;
			ulong allCount = 0;
			//std::cout << "Start Evo mutator with " << threadsSupported << " threads" << std::endl;
			for (uint j = 0; j < threadsSupported; ++j) {
				threadSolutions.push_back(
						std::async(std::launch::async, &innerMutateLocalRandom, population, allCount, allCount + counter[j],settings.innerSASetting,problem));
				allCount += counter[j];
			}
		}
	} else {
		innerMutateLocalRandom(population, 0, population->size(), settings.innerSASetting, problem);
	}
}

Metaheuristic::EvolutionComputation::EvolutionComputation() {
	SASettings *saSettings = &settings.innerSASetting;
	saSettings->maxThreads = 1;
	saSettings->seed = settings.seed + 21168421;
	saSettings->maxTime = 5;
	saSettings->startT = 25;
	saSettings->maxNoIncrementsAndSwaps = 500;
}

Metaheuristic::EvolutionComputation::EvolutionComputation(Metaheuristic::EvolveSettings settings) {
	this->settings = settings;
}
