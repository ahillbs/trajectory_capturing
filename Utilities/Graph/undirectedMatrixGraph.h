//
// Created by gharg on 6/15/17.
//

#ifndef BA_UNDIRECTEDMATRIXGRAPH_H
#define BA_UNDIRECTEDMATRIXGRAPH_H


#include <utility>
#include <vector>
#include <tuple>
#include "undirectedGraph.h"


class undirectedMatrixGraph {
private:
	std::vector<double> matrix;
	unsigned int n;
	unsigned int getMatrixPosition(Vertex start, Vertex end);
public:
	undirectedMatrixGraph();
	undirectedMatrixGraph(unsigned int n);

	unsigned int getSize();
	std::vector<Vertex> getVertices();
	std::vector<Edge> getEdges(bool withoutZeroWeight);
	Edge getEdge(Vertex start, Vertex end);
	bool setEdge(Vertex start, Vertex end, double weight);
};


#endif //BA_UNDIRECTEDGRAPH_H
