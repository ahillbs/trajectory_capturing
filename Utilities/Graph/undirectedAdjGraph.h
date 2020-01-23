//
// Created by gharg on 6/16/17.
//

#ifndef BA_UNDIRECTEDADJGRAPH_H
#define BA_UNDIRECTEDADJGRAPH_H


#include <unordered_set>
#include <vector>
#include <map>
#include <set>
#include "undirectedGraph.h"


class undirectedAdjGraph {
private:
	std::set<Vertex> vertices;
	std::map<Vertex,std::map<Vertex,double>> adjList;

public:
	undirectedAdjGraph();
	undirectedAdjGraph(unsigned int n);

	void clear();
	unsigned long getSize();
	const std::set<Vertex> & getVertices() const;
	std::vector<Edge> getEdges() const;
	std::vector<Edge> getEdges(Vertex v) const;
	std::pair<Edge, bool> getEdge(Vertex start, Vertex end) const;
	bool setEdge(Vertex start, Vertex end, double weight);
	Vertex addVertex();
	bool addVertex(Vertex v);


	undirectedAdjGraph createSubgraph(std::set<Vertex> &subGraphVertices);

	std::pair<Edge, bool> getEdge(std::pair<Vertex, Vertex> &pair);
};
#endif //BA_UNDIRECTEDADJGRAPH_H
