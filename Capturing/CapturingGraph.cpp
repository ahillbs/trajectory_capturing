//
// Created by gharg on 10.07.17.
//

#include "Header/CapturingGraph.h"

using EdgeToCplexPair = std::pair<std::pair<Vertex ,Vertex >,int>;
using VertexPair = std::pair<Vertex,Vertex>;
using SegmentEdge = std::pair<ulong,std::pair<Vertex,Vertex>>;
using EdgeToCplex =  std::map<SegmentEdge,int>;
using CplexToEdge = std::map<ulong,VertexPair >;

CapturingGraph::CapturingGraph() {}

template <typename T, typename U, typename V>
inline std::pair<T,std::pair<U,V>> make_triplet(T t, U u, V v) {
	std::make_pair(t,std::make_pair(u,v));
}

undirectedAdjGraph* CapturingGraph::getGraph() {
	return &this->graph;
}

void CapturingGraph::setGraph(undirectedAdjGraph &graph) {
	this->graph = graph;
}

std::vector<std::vector<Vertex>> *CapturingGraph::getVertexFamilies() {
	return &this->vertexFamilies;
}

void CapturingGraph::setVertexFamilies(std::vector<std::vector<Vertex>> &families) {
	this->vertexFamilies = families;
}



CapturingSolution CapturingGraph::computeSolution(std::set<Vertex> &solution) {
	double length = 0;
	std::vector<Edge> edges;
	for (unsigned int i = 0; i < vertexFamilies.size(); ++i) {
		/*
		Vertex startVertex = Vertex() - 1;
		Vertex endVertex = Vertex() - 1;
		unsigned long j = 0;
		unsigned long k = vertexFamilies[i].size() - 1;
		while(k > j && (endVertex == Vertex() - 1 || startVertex == Vertex() - 1)) {
			if(startVertex == Vertex() - 1 && (solution.find(vertexFamilies[i][j]) != solution.end())) {
				startVertex = vertexFamilies[i][j];
			} else if (startVertex == Vertex() - 1){
				++j;
			}
			if(endVertex == Vertex() - 1 && (solution.find(vertexFamilies[i][k]) != solution.end())) {
				endVertex = vertexFamilies[i][k];
			} else if (endVertex == Vertex() - 1){
				--k;
			}

		}*/
		std::pair<unsigned int, unsigned int> indexes = getFamilyIndexPair(i,solution);
		//found a segment track
		unsigned int j = indexes.first;
		unsigned int k = indexes.second;
		if(k > j) {
			for (unsigned long l = j+1; l <= k ; ++l) {
				// add every edge to the total length
				Edge edge = graph.getEdge(vertexFamilies[i][l-1],vertexFamilies[i][l]).first;
				edges.push_back(edge);
				length += edge.third;
			}
		}
	}
	return CapturingSolution(solution, edges, length, this);
}

/// Returns the two family indexes, which have the longest weight/path
/// \param familyIndex Index of the Family searching
/// \param solution solution to be used in the family
/// \return pair of indexes in the family
std::pair<unsigned int, unsigned int> CapturingGraph::getFamilyIndexPair(unsigned long familyIndex,
																		 const std::set<Vertex> &solution) {
	Vertex startVertex = Vertex() - 1;
	Vertex endVertex = Vertex() - 1;
	unsigned long j = 0;
	unsigned long k = vertexFamilies[familyIndex].size() - 1;
	while(k > j && (endVertex == Vertex() - 1 || startVertex == Vertex() - 1)) {
		if(startVertex == Vertex() - 1 &&
				(solution.find(vertexFamilies[familyIndex][j]) != solution.end())) {
			startVertex = vertexFamilies[familyIndex][j];
		} else if (startVertex == Vertex() - 1) {
			++j;
		}
		if(endVertex == Vertex() - 1 &&
				(solution.find(vertexFamilies[familyIndex][k]) != solution.end())) {
			endVertex = vertexFamilies[familyIndex][k];
		} else if (endVertex == Vertex() - 1 ) {
			--k;
		}
	}
	return std::pair<unsigned int, unsigned int>(j,k);
}

double CapturingGraph::getWeightInFamily(unsigned long familyIndex, Vertex v1, Vertex v2) {
	std::set<Vertex> pairSolution = {v1,v2};
	auto indexes = getFamilyIndexPair(familyIndex,pairSolution);
	unsigned int j = indexes.first;
	unsigned int k = indexes.second;
	if(k > j) {
		double length = 0;
		for (unsigned long l = j+1; l <= k ; ++l) {
			// add every edge to the total length
			Edge edge = graph.getEdge(vertexFamilies[familyIndex][l-1],vertexFamilies[familyIndex][l]).first;
			length += edge.third;
		}
		return length;
	}
	return 0;
}

double CapturingGraph::getWeightInFamilyWithIndex(unsigned long familyIndex, unsigned long index1, unsigned long index2) {
	if (index2 < index1) {
		return 0;
	}
	double length = 0;
	for (unsigned long l = index1 + 1; l <= index2; ++l) {
		// add every edge to the total length
		Edge edge = graph.getEdge(vertexFamilies[familyIndex][l - 1], vertexFamilies[familyIndex][l]).first;
		length += edge.third;
	}
	return length;

}

std::map<Vertex, std::set<Vertex>> & CapturingGraph::getVertexInFamiliesMapping() {
	return vertexInFamiliesMapping;
}




