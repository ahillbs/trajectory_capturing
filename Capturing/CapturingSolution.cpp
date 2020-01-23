//
// Created by gharg on 27.08.17.
//

#include <bitset>
#include <functional>
#include <future>
#include "Header/CapturingSolution.h"
#include "Header/CapturingGraph.h"
#include "Neighborhoods/OuterSharedSegmentsVertices.h"
#define LOG

void CapturingSolution::setSolutionValue(double solutionValue) {
	CapturingSolution::solutionValue = solutionValue;
}

CapturingSolution::CapturingSolution(CapturingGraph *capGraph) {
	this->capGraph = capGraph;
}

CapturingSolution::CapturingSolution(std::set<Vertex> &vertexSolution, std::vector<Edge> &edgeSolution, double value,
									 CapturingGraph *capGraph) {
	this->vertexSolution = vertexSolution;
	this->edgeSolution = edgeSolution;
	this->solutionValue = value;
	this->capGraph = capGraph;
}

CapturingSolution::CapturingSolution(const std::set<Vertex> vertexSolution,CapturingGraph *capGraph) {
	this->vertexSolution = vertexSolution;
	this->capGraph = capGraph;
	this->isEdgeSolutionInvalid = true;
	this->computeSolution(false,true);
}

const std::set<Vertex> &CapturingSolution::getVertexSolution() const {
	return vertexSolution;
}

void CapturingSolution::setVertexSolution(const std::set<Vertex> &vertexSolution) {
	CapturingSolution::vertexSolution = vertexSolution;
	isEdgeSolutionInvalid = true;
}

const std::vector<Edge> & CapturingSolution::getEdgeSolution() const{
	return edgeSolution;
}

const std::vector<Edge> & CapturingSolution::getEdgeSolution() {
    if(isEdgeSolutionInvalid) {
        computeSolution(false, false);
    }
    return edgeSolution;
}

void CapturingSolution::setEdgeSolution(const std::vector<Edge> &edgeSolution) {
	CapturingSolution::edgeSolution = edgeSolution;
	isEdgeSolutionInvalid = false;
}

double CapturingSolution::getSolutionValue() const {
	return solutionValue;
}



std::set<Vertex>& CapturingSolution::getVertexSolution() {
	return this->vertexSolution;
}

std::vector<std::set<Vertex>> getPermutations(std::set<Vertex> &iniSet, std::set<Vertex>::iterator offset, unsigned int k) {

	std::vector<std::set<Vertex>> permSet;

	for (auto i = offset; i != iniSet.end(); ++i) {
		Vertex v = *i;
		if(k>1) {
			auto otherPermutations = getPermutations(iniSet, ++i, k - 1);
			--i;
			for (uint j = 0; j < otherPermutations.size(); ++j) {
				otherPermutations[j].insert(v);
				permSet.push_back(otherPermutations[j]);
			}
		} else {
			std::set<Vertex> vertSet;
			vertSet.insert(v);
			permSet.push_back(vertSet);
		}
	}
	return permSet;
}

/// looks for all possible neighbourhoods with k vertices swapped
/// \param capGraph solution based graph
/// \param k amount of swapped elements in solution
/// \return neighbourhoods with k vertices swapped
std::vector<CapturingSolution> CapturingSolution::getNeighbourhood(unsigned int k) {
	std::vector<CapturingSolution> solution;
	std::vector<std::set<Vertex>> allPermVertSol = getPermutations(this->vertexSolution,this->vertexSolution.begin(),k);
	//sets the computer on fire... too many variations
	//std::vector<std::set<Vertex>> allPermVert = getPermutations(allNodes,allNodes.begin(),k);

	std::cout << std::endl;
	for (auto l = this->vertexSolution.begin(); l != this->vertexSolution.end(); ++l) {
		std::cout << *l << ",";
	}
	std::cout << std::endl;

	for (unsigned int i = 0; i < allPermVertSol.size(); ++i) {
		CapturingSolution sol = *this;
		 std::cout << "Permutation " << i << std::endl;
		for (auto j = allPermVertSol[i].begin(); j != allPermVertSol[i].end() ; ++j) {
			std::cout << *j << ",";
			sol.vertexSolution.erase(*j);
		}
		std::cout << std::endl;

		//Swap vertex at position 1 in Solution with another vertex not in solution

		std::cout << "after erase: " << std::endl;
		for (auto j = sol.vertexSolution.begin(); j != sol.vertexSolution.end(); ++j) {
			std::cout << *j << ",";
		}
		std::cout << std::endl;
	}
	return solution;
}
#define BITSIZE 64
CapturingSolution CapturingSolution::iteratedLocalSearch(bool localNeighbour = false) {
	CapturingSolution bestSol = *this;

	const unsigned long bitsetSize = capGraph->getGraph()->getSize()/BITSIZE + 1;
	std::vector<std::bitset<BITSIZE>> solutionBitset(bitsetSize);
	//translate vertices to bitset
	for (auto i = this->vertexSolution.begin(); i != vertexSolution.end(); ++i) {
		//separate array offset and internal position
		unsigned long offset = (*i)/BITSIZE;
		unsigned long internal = (*i) % BITSIZE;
		solutionBitset[offset][internal] = 1;
	}
	double innerBestSolValue = bestSol.solutionValue;
	CapturingSolution innerBestSol = bestSol;


	unsigned int iteration = 0;
	do {
		bestSol = innerBestSol;
#ifdef LOG
		//std::cout << innerBestSol.vertexSolution.size() << "  = amount vertex in solution" << std::endl;
		std::cout << "iteration: " << iteration << " best solution: " << bestSol.getSolutionValue() << std::endl;
#endif
		// fix one solution vertex and swap him with every other vertex in the graph
		for (auto i = bestSol.vertexSolution.begin(); i != bestSol.vertexSolution.end(); ++i) {
			//unsigned long offset = (*i) / BITSIZE;
			//unsigned long internal = (*i) % BITSIZE;
			//set bit i to 0 and other to 1
			//solutionBitset[offset][internal] = 0;
			CapturingSolution s = bestSol;
			if(s.vertexSolution.find(*i) == s.vertexSolution.end()) {
				std::cout << "try to delete vertex, that isnt in solution" << std::endl;
			}
			s.deleteVertex(*i);
			std::vector<std::bitset<BITSIZE>> testVerticesBitset(bitsetSize);
			std::set<Vertex> relatedVertices;
			if(localNeighbour) {
				buildRelatedVertices(s,*i,&relatedVertices);
				for(auto v : relatedVertices) {
					ulong offset = v / BITSIZE;
					ulong internal = v % BITSIZE;
					testVerticesBitset[offset][internal] = 1;
				}
			} else {
				ulong max = this->capGraph->getGraph()->getSize();
				for (unsigned long j = 0; j < bitsetSize ; ++j) {
					for (unsigned long k = 0; k < BITSIZE; ++k) {
						if((j*BITSIZE + k) < max && solutionBitset[j][k] == 0)
							testVerticesBitset[j][k] = 1;
					}
				}
			}

			for (unsigned int j = 0; j < bitsetSize ; ++j) {
				for (unsigned int k = 0; k < BITSIZE; ++k) {
					if (testVerticesBitset[j][k] == 1) {
						CapturingSolution s2 = s;
						s2.addVertex(j * BITSIZE + k);
						if (s2.solutionValue > innerBestSol.solutionValue) {
#ifdef LOG
							//std::cout << "found better solution: "<< s2.solutionValue << std::endl;
#endif
							//innerBestSol = s2;
							innerBestSolValue = s2.solutionValue;
							innerBestSol.vertexSolution.clear();
							for(auto innerV : s2.vertexSolution) {
								innerBestSol.vertexSolution.insert(innerV);
							}
							innerBestSol.solutionValue = innerBestSolValue;
						}
					}
				}
			}

			//reset to original state
			//solutionBitset[offset][internal] = 1;
		}
		++iteration;


		for (uint l = 0; l < solutionBitset.size(); ++l) {
			solutionBitset[l].reset();
		}
		for (auto i = innerBestSol.vertexSolution.begin(); i != innerBestSol.vertexSolution.end(); ++i) {
			//separate array offset and internal position
			unsigned long offset = (*i)/BITSIZE;
			unsigned long internal = (*i) % BITSIZE;
			solutionBitset[offset][internal] = 1;
		}
	} while (innerBestSolValue != bestSol.solutionValue);
	innerBestSol.computeSolution(false,true);
	return innerBestSol;
}

void CapturingSolution::deleteVertex(Vertex v) {
	// Do nothing if vertex is not in solution
	if (vertexSolution.find(v) == vertexSolution.end()) {
		return;
	}
	isEdgeSolutionInvalid = true;

	auto &vertexFamilyMapping = capGraph->getVertexInFamiliesMapping();
	double objValue = 0;
	// subtract value that are in the corresponding segments/families
	for (auto i = vertexFamilyMapping[v].begin(); i != vertexFamilyMapping[v].end(); ++i) {
		unsigned long familyIndex = *i;
		auto indexPair = capGraph->getFamilyIndexPair(familyIndex,this->vertexSolution);
		if(indexPair.first < indexPair.second) {
			objValue += capGraph->getWeightInFamilyWithIndex(familyIndex, indexPair.first, indexPair.second);
		}
	}
	this->solutionValue -= objValue;
	vertexSolution.erase(v);
	objValue = 0;
	// after deletion of the vertex we need to add value of the corresponding segments, in case
	// two other vertices are on the same segments and capture value
	for (auto i = vertexFamilyMapping[v].begin(); i != vertexFamilyMapping[v].end(); ++i) {
		unsigned long familyIndex = *i;
		auto indexPair = capGraph->getFamilyIndexPair(familyIndex,this->vertexSolution);
		if(indexPair.first < indexPair.second) {
			objValue += capGraph->getWeightInFamilyWithIndex(familyIndex, indexPair.first, indexPair.second);
		}
	}
	this->solutionValue += objValue;
}

void CapturingSolution::addVertex(Vertex v) {
	// Do nothing if vertex is already in solution
	if (vertexSolution.find(v) != vertexSolution.end() || v > this->capGraph->getGraph()->getSize()) {
		return;
	}
	isEdgeSolutionInvalid = true;
	auto &vertexFamilyMapping = capGraph->getVertexInFamiliesMapping();
	double objValue = 0;
	// subtract value that are in the corresponding segments/families
	for (auto i = vertexFamilyMapping[v].begin(); i != vertexFamilyMapping[v].end(); ++i) {
		unsigned long familyIndex = *i;
		auto indexPair = capGraph->getFamilyIndexPair(familyIndex,this->vertexSolution);
		if(indexPair.first < indexPair.second) {
			objValue += capGraph->getWeightInFamilyWithIndex(familyIndex, indexPair.first, indexPair.second);
		}
	}
	this->solutionValue -= objValue;
	vertexSolution.insert(v);
	objValue = 0;
	// after deletion of the vertex we need to add value of the corresponding segments, in case
	// two other vertices are on the same segments and capture value
	for (auto i = vertexFamilyMapping[v].begin(); i != vertexFamilyMapping[v].end(); ++i) {
		unsigned long familyIndex = *i;
		auto indexPair = capGraph->getFamilyIndexPair(familyIndex,this->vertexSolution);
		if(indexPair.first < indexPair.second) {
			objValue += capGraph->getWeightInFamilyWithIndex(familyIndex, indexPair.first, indexPair.second);
		}
	}
	this->solutionValue += objValue;
}

void CapturingSolution::computeSolution(bool include_edge_solution, bool include_solution_value) {
	if(!(include_edge_solution || include_solution_value))
		return;
	if(include_edge_solution)
		edgeSolution.clear();
	if(include_solution_value)
		solutionValue = 0;
	auto *vertexFamilies = capGraph->getVertexFamilies();
	auto *graph = capGraph->getGraph();
	for (unsigned int i = 0; i < vertexFamilies->size(); ++i) {
		unsigned int familyIndex = i;
		auto indexPair = capGraph->getFamilyIndexPair(familyIndex,this->vertexSolution);
		for (uint j = indexPair.first; j < indexPair.second; ++j) {
			auto e = graph->getEdge((*vertexFamilies)[i][j],(*vertexFamilies)[i][j+1]);
			if(e.second) {
				if(include_edge_solution)
					edgeSolution.push_back(e.first);
				if(include_solution_value)
					solutionValue += e.first.third;
			}
		}
	}
}



CapturingGraph *CapturingSolution::getGraph() const {
	return this->capGraph;
}

CapturingSolution CapturingSolution::getRandomLocalNeighbour(ulong seed, Neighborhood *neighborhood) {
	CapturingSolution randNeighbour = *this;

	for (auto i = randNeighbour.vertexSolution.begin(); i != randNeighbour.vertexSolution.end(); ++i) {
		seed += *i;
	}
	seed += randNeighbour.getSolutionValue();

	// some variables added to the seed to make it "random"
	std::mt19937_64 random(seed);
	std::uniform_int_distribution<ulong> distribution;
	Vertex removedVertex;
	removeRandomVertex(randNeighbour, random, distribution, removedVertex);
	std::unordered_set<Vertex> neighborhoodVertices;
	neighborhood->getNeighborhood(randNeighbour,&neighborhoodVertices);
	addRandRelVertToSol(randNeighbour, random, distribution, neighborhoodVertices);

	return CapturingSolution(std::set<Vertex>(), nullptr);
}

CapturingSolution CapturingSolution::getRandomLocalNeighbour(ulong seed,bool localSearch) {
	CapturingSolution randNeighbour = *this;

	for (auto i = randNeighbour.vertexSolution.begin(); i != randNeighbour.vertexSolution.end(); ++i) {
		seed += *i;
	}
	seed += randNeighbour.getSolutionValue();

	// some variables added to the seed to make it "random"
	std::mt19937_64 random(seed);
	std::uniform_int_distribution<ulong> distribution;
	Vertex removedVertex;
	removeRandomVertex(randNeighbour, random, distribution, removedVertex);

	// look for closer related Vertices
	if (localSearch) {
		std::set<Vertex> relatedVertices;
		buildRelatedVertices(randNeighbour, removedVertex, &relatedVertices);

		addRandRelVertToSol(randNeighbour, random, distribution, relatedVertices);
	} else {
		addRandVertToSol(randNeighbour, random, distribution);
	}
	return randNeighbour;
}

void CapturingSolution::addRandVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
										 std::uniform_int_distribution<ulong> &distribution) const {
	std::uniform_int_distribution<ulong>::param_type distrBoundaries(0, randNeighbour.vertexSolution.size());
	Vertex insertedVertex = distribution(random,distrBoundaries);
	while (randNeighbour.vertexSolution.find(insertedVertex) != randNeighbour.vertexSolution.end()) {
			insertedVertex = distribution(random,distrBoundaries);
		}
	randNeighbour.addVertex(insertedVertex);
}

void CapturingSolution::addRandRelVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
											std::uniform_int_distribution<ulong> &distribution,
											const std::set<Vertex> &relatedVertices) const {
	std::uniform_int_distribution<ulong>::param_type distrBoundariesRel(0, relatedVertices.size() - 1);
	Vertex insertedVertex = distribution(random,distrBoundariesRel);

	auto it = relatedVertices.begin();
	for (uint j = 0; j < insertedVertex; ++j) {
		++it;
	}

	insertedVertex = *it;
	randNeighbour.addVertex(insertedVertex);
}

void CapturingSolution::addRandRelVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
											std::uniform_int_distribution<ulong> &distribution,
											const std::unordered_set<Vertex> &relatedVertices) const {
	std::uniform_int_distribution<ulong>::param_type distrBoundariesRel(0, relatedVertices.size() - 1);
	Vertex insertedVertex = distribution(random,distrBoundariesRel);

	auto it = relatedVertices.begin();
	for (uint j = 0; j < insertedVertex; ++j) {
		++it;
	}

	insertedVertex = *it;
	randNeighbour.addVertex(insertedVertex);
}

void CapturingSolution::buildRelatedVertices(const CapturingSolution &Neighbour, Vertex removedVertex,
											 std::set<Vertex> *relatedVertices) const {
	std::map<Vertex, std::set<Vertex>>& familiesMapping = Neighbour.capGraph->getVertexInFamiliesMapping();
	std::vector<std::vector<Vertex>>& families = *Neighbour.capGraph->getVertexFamilies();

	for (Vertex v : Neighbour.vertexSolution) {
		for (Vertex segIndex : familiesMapping[v])
		{
			for (uint i = 0; i < families[segIndex].size(); ++i) {
				relatedVertices->insert(families[segIndex][i]);
			}
		}
	}
	// remove already inserted or the removed Vertices
	relatedVertices->erase(removedVertex);
	for (auto v : Neighbour.vertexSolution) {
		relatedVertices->erase(v);
	}
}

void CapturingSolution::removeRandomVertex(CapturingSolution &randNeighbour, std::mt19937_64 &random,
										   std::uniform_int_distribution<ulong> &distribution, Vertex &removedVertex) const {
	std::uniform_int_distribution<ulong>::param_type distrBoundariesSol(0, randNeighbour.vertexSolution.size() - 1);
	removedVertex= distribution(random, distrBoundariesSol);
	auto it1 = randNeighbour.vertexSolution.begin();
	for (uint j = 0; j < removedVertex; ++j) {
		++it1;
	}
	removedVertex = *it1;

	randNeighbour.deleteVertex(removedVertex);
}

bool CapturingSolution::checkFeasible() {
	
	auto* vertFamilies = capGraph->getVertexFamilies();
	//auto& vertFamilyMapping = capGraph->getVertexInFamiliesMapping();
	auto* graph = capGraph->getGraph();
	std::vector<Edge> edges;
	double weight = 0;
	for (uint i = 0; i < vertFamilies->size(); ++i) {
		auto indexPair = this->capGraph->getFamilyIndexPair(i,this->vertexSolution);
		if (indexPair.first < indexPair.second) {
			for (uint j = indexPair.first; j < indexPair.second; ++j) {
				auto edgePair = graph->getEdge((*vertFamilies)[i][j],(*vertFamilies)[i][j+1]);
				if(edgePair.second) {
					edges.push_back(edgePair.first);
					weight += edgePair.first.third;
				}
			}
		}
	}
	if(edges.size() != getEdgeSolution().size()) {
		return false;
	}
	for (uint l = 0; l < edges.size(); ++l) {
		bool foundEdge = false;
		for (uint i = 0; i < edgeSolution.size(); ++i) {
			if(edges[l].first == edgeSolution[i].first && edges[l].second == edgeSolution[i].second) {
				if (edges[l].third == edgeSolution[i].third) {
					foundEdge = true;
					break;
				}
			}
		}
		if(!foundEdge) {
			return false;
		}
	}
	return true;
}

CapturingSolution CapturingSolution::getBestNeighbourSolution(std::unordered_set<double> *tabuSet,bool localSearch) {

	CapturingSolution bestSol(this->capGraph);
	bestSol.solutionValue = 0;
	//std::cout << "call getBestNeighbourSol " << bestSol.getSolutionValue() << std::endl;
	for(auto removedVertex : this->getVertexSolution()) {
		CapturingSolution currSol = *this;
		currSol.deleteVertex(removedVertex);
		if(localSearch) {
			std::set<Vertex> relatedVertices;
			buildRelatedVertices(currSol, removedVertex, &relatedVertices);
			for (auto insertedVertex : relatedVertices) {
				CapturingSolution inCurrSol = currSol;
				inCurrSol.addVertex(insertedVertex);
				if (tabuSet->find(inCurrSol.getSolutionValue()) == tabuSet->end()) {
					if (inCurrSol.getSolutionValue() > bestSol.getSolutionValue()) {
						//std::cout << "found better Sol in BestNeighbour: " << inCurrSol.getSolutionValue() << std::endl;
						bestSol = inCurrSol;
					}
				}
			}
		} else {
			for (Vertex insertedVertex = 0; insertedVertex < this->capGraph->getGraph()->getSize(); ++insertedVertex) {
				CapturingSolution inCurrSol = currSol;
				inCurrSol.addVertex(insertedVertex);
				if (tabuSet->find(inCurrSol.getSolutionValue()) == tabuSet->end()) {
					if (inCurrSol.getSolutionValue() > bestSol.getSolutionValue()) {
						//std::cout << "found better Sol in BestNeighbour: " << inCurrSol.getSolutionValue() << std::endl;
						bestSol = inCurrSol;
					}
				}
			}
		}
	}
	return bestSol;
}

double CapturingSolution::getUpperValue() const {
	return upperValue;
}

void CapturingSolution::setUpperValue(double upperValue) {
	CapturingSolution::upperValue = upperValue;
}

CapturingSolution CapturingSolution::iteratedLocalSearch(Neighborhood *neighborhood) {
	std::cout << "Start neighborhood Iterated Search" << std::endl;
	CapturingSolution bestSol = *this;
	CapturingSolution innerBestSol = bestSol;
	ulong iteration = 0;
	do {
		++iteration;
		if(bestSol.getSolutionValue() < innerBestSol.getSolutionValue()) {
			bestSol = innerBestSol;
		}
		std::cout << "iteration: " << iteration << " best solution: " << bestSol.getSolutionValue() << std::endl;
		//innerBestSol = bestSol;
		//std::cout << "call getBestNeighbourSol " << innerBestSol.getSolutionValue() << std::endl;
		for (auto removedVertex : innerBestSol.getVertexSolution()) {
			CapturingSolution currSol = innerBestSol;
			currSol.deleteVertex(removedVertex);

			std::unordered_set<Vertex> neighborhoodVertices;
			neighborhood->getNeighborhood(currSol, &neighborhoodVertices);
			for (auto insertedVertex : neighborhoodVertices) {
				CapturingSolution inCurrSol = currSol;
				inCurrSol.addVertex(insertedVertex);
				if (inCurrSol.getSolutionValue() > innerBestSol.getSolutionValue()) {
					//std::cout << "found better Sol in BestNeighbour: " << inCurrSol.getSolutionValue() << std::endl;
					innerBestSol = inCurrSol;
				}
			}
		}

	} while(bestSol.getSolutionValue() < innerBestSol.getSolutionValue());
	return bestSol;
}


