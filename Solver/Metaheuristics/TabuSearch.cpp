//
// Created by gharg on 26.09.17.
//

#include <unordered_map>
#include "TabuSearch.h"
#include "../Solver/GreedySolver.h"

using Vertices = std::vector<Vertex>;
ulong constructKey(const std::vector<Vertex>& sol) {
	ulong retVal = 0;
	for(auto v : sol) retVal += v;
	return retVal;
}

template <typename array1,typename array2>
bool isSameSet(const array1* vert1, const array2* vert2) {
	//it is assumed that the arrays are sorted as they come from a std::set
	if(vert1->size() != vert2->size())
		return false;
	auto it1 = vert1->begin();
	auto it2 = vert2->begin();
	while (it1 != vert1->end()) {
		if(*it1 != *it2) {
			return false;
		}
		++it1;
		++it2;
	}
	return true;
}

template <typename array1,typename array2>
void transportTo(const array1* vert1, array2* vert2) {
	//it is assumed that the arrays are sorted as they come from a std::set
	vert2->clear();
	auto it1 = vert1->begin();
	auto it2 = vert2->begin();
	while (it1 != vert1->end()) {
		*it2 = *it1;
		++it1;
		++it2;
	}
}

void insertIntoVector(Vertices* verts, const std::set<Vertex>& solSet) {
	verts->clear();
	for(auto v : solSet) {
		verts->push_back(v);
	}
}

CapturingSolution Metaheuristic::TabuSearch::solve(Capturing2D &problem, ulong maxK) {
	problem.convertSegments2Graph();
	auto begin_time = std::chrono::high_resolution_clock::now();
	GreedySolver gSolver;
	CapturingSolution bestSol = gSolver.solve(&problem,maxK);

	CapturingSolution currSol = bestSol;
	//only remember solution value as identifying key
	std::vector<double> tabuList(memoryLength);
	std::unordered_set<double> tabuSet;

	ulong iteration = 0;
	ulong maxIteration = 1000;
	ulong noImproveCount = 0;
	ulong maxNoImprove = 250;
	tabuList[0] = currSol.getSolutionValue();
	tabuSet.insert(tabuList[0]);


	ulong seconds;
	seconds = (ulong)std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::high_resolution_clock::now() - begin_time).count();

	while(iteration < maxIteration && noImproveCount < maxNoImprove && seconds < maxTime) {
		++iteration;
		++noImproveCount;
		currSol = currSol.getBestNeighbourSolution(&tabuSet);
		if(std::min(bestSol.getSolutionValue()*1.01,bestSol.getSolutionValue()+1) < currSol.getSolutionValue()) {
			bestSol = currSol;
			noImproveCount = 0;
			std::stringstream str;
			str << std::fixed << std::setprecision( 15 ) << currSol.getSolutionValue();
			std::cout << "i: " << iteration << " improvement bestSol: " << str.str() << std::endl;
			std::cout << "bestSol Vertices: ";
			for(auto v : bestSol.getVertexSolution())
				std::cout << std::to_string(v) << ",";
			std::cout << std::endl;
		}
		updateTabuList(currSol, tabuList, tabuSet, iteration);

		seconds = (ulong)std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::high_resolution_clock::now() - begin_time).count();
	}
	//std::cout << "iterations at the end: " << iteration << std::endl;

	return bestSol;
}

void Metaheuristic::TabuSearch::updateTabuList(const CapturingSolution &currSol, std::vector<double> &tabuList,
								std::unordered_set<double> &tabuSet, ulong iteration) {
	if(tabuSet.find(tabuList[iteration % memoryLength]) != tabuSet.end()) {
		//std::cout << "erase called. i: " << iteration << "tabuList[]: " << tabuList[iteration % memoryLength] << std::endl;
			tabuSet.erase(tabuList[iteration % memoryLength]);
	}
	tabuList[iteration % memoryLength] = currSol.getSolutionValue();
	tabuSet.insert(currSol.getSolutionValue());
	/*if((iteration > 20000 && tabuSet.size() != 20000) || (iteration <= 20000 && tabuSet.size() != iteration)) {
		std::cout << "iteration and tabuSet size are not equal! i: " << iteration << " set: " << tabuSet.size() << std::endl;
	}*/
}

ulong Metaheuristic::TabuSearch::getMaxTime() const {
	return maxTime;
}

void Metaheuristic::TabuSearch::setMaxTime(ulong maxTime) {
	TabuSearch::maxTime = maxTime;
}
