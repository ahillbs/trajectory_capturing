//
// Created by gharg on 10.07.17.
//

#ifndef BA_CAPTURINGGRAPH_H
#define BA_CAPTURINGGRAPH_H

#include <set>
#include "../../cplex/cplex.hpp"
#include "../../Utilities/Graph/undirectedAdjGraph.h"
#include "CapturingSolution.h"


class CapturingGraph {


protected:
	undirectedAdjGraph graph;
	std::vector<std::vector<Vertex>> vertexFamilies;
	std::map<Vertex, std::set<Vertex>> vertexInFamiliesMapping;

public:
	CapturingGraph();

	std::map<Vertex, std::set<Vertex>> & getVertexInFamiliesMapping();


	undirectedAdjGraph *getGraph();
	void setGraph(undirectedAdjGraph &graph);
	std::vector<std::vector<Vertex>> *getVertexFamilies();
	void setVertexFamilies(std::vector<std::vector<Vertex>> &families);
	CapturingSolution computeSolution(std::set<Vertex> &solution);


	std::pair<unsigned int, unsigned int> getFamilyIndexPair(unsigned long familyIndex,
															 const std::set<Vertex> &solution);
	double getWeightInFamily(unsigned long familyIndex, Vertex v1, Vertex v2);
	double getWeightInFamilyWithIndex(unsigned long familyIndex, unsigned long index1, unsigned long index2);


};


#endif //BA_CAPTURINGGRAPH_H
