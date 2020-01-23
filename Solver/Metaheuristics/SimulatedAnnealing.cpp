//
// Created by gharg on 26.09.17.
//

#include <random>
#include <functional>
#include <thread>
#include <future>
#include "SimulatedAnnealing.h"
#include "../Solver/GreedySolver.h"

void Metaheuristic::computeSolution(CapturingSolution *sol, ulong innerSeed, SASettings settings) {
	//std::cout << "Start Simulated Annealing " << innerSeed - settings.seed << std::endl;
	auto begin_time = std::chrono::high_resolution_clock::now();
	double T = settings.startT * 0.5;
	CapturingSolution bestSol = *sol;
	CapturingSolution currentSol = *sol;
	ulong i = 0;
	std::mt19937_64 random(innerSeed);
	//auto dice_randX = std::bind(std::uniform_int_distribution<uint>(0, 100), random);
	std::bernoulli_distribution distribution;
	ulong noIncrementsAndSwaps = 0;
	ulong noIncrements = 0;
	uint seconds = (uint)std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::_V2::system_clock::now() - begin_time).count();
	while (i < settings.maxIteration && noIncrements < settings.maxNoIncrements && seconds < settings.maxTime) {
		++noIncrementsAndSwaps;
		++noIncrements;
		CapturingSolution innerSol = currentSol;
		if(settings.neighborhood != nullptr) {
			innerSol = currentSol.getRandomLocalNeighbour(i, settings.useLocal);
		} else {
			innerSol = currentSol.getRandomLocalNeighbour(i, settings.neighborhood);
		}
		if (innerSol.getSolutionValue() > currentSol.getSolutionValue()) {
			//std::cout << "found better solution to current: " << innerSol.getSolutionValue() <<" iteration: " << i <<  std::endl;
			currentSol = innerSol;
			noIncrementsAndSwaps = 0;
			if (innerSol.getSolutionValue() > bestSol.getSolutionValue()) {
				//std::cout << "Thread: "<< innerSeed - settings.seed <<" found better solution to overall: " << innerSol.getSolutionValue() << " iteration: " << i << std::endl;
				bestSol = innerSol;
				noIncrements = 0;
			}
		} else {
			const double prob = settings.probabiltyFunc(currentSol.getSolutionValue(), innerSol.getSolutionValue(), T);
			std::bernoulli_distribution::param_type t(std::min(prob, 100.D));

			if(distribution(random,t)) {
				noIncrementsAndSwaps = 0;
				//std::cout << "swapped curr solution to: " << innerSol.getSolutionValue() <<" iteration: " << i << std::endl;
				currentSol = innerSol;
			}
		}
		//no increment in a long time => reset Temperature for more wandering
		if(noIncrementsAndSwaps > settings.maxNoIncrementsAndSwaps) {
			T = settings.startT;
		}
		T = settings.temperatureFunc(T, i);
		seconds = std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::_V2::system_clock::now() - begin_time).count();
		++i;
	}
	std::cout << "simAnneal iterations: " << i << " bestSoluton: " << bestSol.getSolutionValue()  << std::endl;
	bestSol.computeSolution(false,true);
	*sol = bestSol;
}
CapturingSolution Metaheuristic::SimulatedAnnealing::solve(Capturing2D *problem, ulong maxK) {
	return solve(problem, maxK, nullptr);
}

CapturingSolution
Metaheuristic::SimulatedAnnealing::solve(Capturing2D *problem, ulong maxK, const CapturingSolution *paramSol) {
	problem->convertSegments2Graph();
	if(settings.temperatureFunc == nullptr) {
		settings.temperatureFunc = temperatureSlowDown05NoReheatFunction;
	}
	if(settings.probabiltyFunc == nullptr) {
		settings.probabiltyFunc = bolzmannFunction;
	}

	CapturingSolution sol(problem);
	if(paramSol == nullptr) {
		GreedySolver solver;
		sol = solver.solve(problem,maxK);
	} else {
		sol = *paramSol;
	}

	unsigned threadsSupported = std::min(std::thread::hardware_concurrency(),settings.maxThreads);

	if(threadsSupported > 1) {
		std::vector<CapturingSolution> solutions;
		for (uint k = 0; k < threadsSupported; ++k) {
			solutions.push_back(sol);
		}
		{
			std::vector<std::future<void>> threadSolutions;
			std::cout << "Start Simualted Annealing with " << threadsSupported << " threads" << std::endl;
			for (uint j = 0; j < threadsSupported; ++j) {
				threadSolutions.push_back(
						std::async(std::launch::async,&Metaheuristic::computeSolution, &solutions[j], settings.seed + j, settings));
			}
			std::cout << "Started all threads" << std::endl;
		}
		CapturingSolution bestSol(problem);
		for (uint j = 0; j < threadsSupported; ++j) {
			//std::cout << "Get " << j << ": " << solutions[j].getSolutionValue() << std::endl;
			if (bestSol.getSolutionValue() < solutions[j].getSolutionValue()) {
				bestSol = solutions[j];
			}
		}
		return bestSol;
	} else {

		computeSolution(&sol, settings.seed, this->settings);
		return sol;
	}
}

ulong Metaheuristic::SimulatedAnnealing::getSeed() const {
	return settings.seed;
}

void Metaheuristic::SimulatedAnnealing::setSeed(ulong seed) {
	SimulatedAnnealing::settings.seed = seed;
}

bool Metaheuristic::SimulatedAnnealing::isUseLocal() const {
	return settings.useLocal;
}

void Metaheuristic::SimulatedAnnealing::setUseLocal(bool useLocal) {
	SimulatedAnnealing::settings.useLocal = useLocal;
}

ulong Metaheuristic::SimulatedAnnealing::getMaxTime() const {
	return settings.maxTime;
}

void Metaheuristic::SimulatedAnnealing::setMaxTime(ulong maxTime) {
	SimulatedAnnealing::settings.maxTime = maxTime;
}

ulong Metaheuristic::SimulatedAnnealing::getMaxIteration() const {
	return settings.maxIteration;
}

void Metaheuristic::SimulatedAnnealing::setMaxIteration(ulong maxIteration) {
	settings.maxIteration = maxIteration;
}

double Metaheuristic::SimulatedAnnealing::getStartT() const {
	return settings.startT;
}

void Metaheuristic::SimulatedAnnealing::setStartT(double startT) {
	settings.startT = startT;
}

ulong Metaheuristic::SimulatedAnnealing::getMaxNoIncrements() const {
	return settings.maxNoIncrements;
}

void Metaheuristic::SimulatedAnnealing::setMaxNoIncrements(ulong maxNoIncrements) {
	settings.maxNoIncrements = maxNoIncrements;
}

ulong Metaheuristic::SimulatedAnnealing::getMaxNoIncrementsAndSwaps() const {
	return settings.maxNoIncrementsAndSwaps;
}

void Metaheuristic::SimulatedAnnealing::setMaxNoIncrementsAndSwaps(ulong maxNoIncrementsAndSwaps) {
	settings.maxNoIncrementsAndSwaps = maxNoIncrementsAndSwaps;
}

const Metaheuristic::SASettings &Metaheuristic::SimulatedAnnealing::getSettings() const {
	return settings;
}

void Metaheuristic::SimulatedAnnealing::setSettings(const Metaheuristic::SASettings &settings) {
	SimulatedAnnealing::settings = settings;
}
