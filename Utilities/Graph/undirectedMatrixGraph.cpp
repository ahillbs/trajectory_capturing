//
// Created by gharg on 6/15/17.
//

#include "undirectedMatrixGraph.h"

undirectedMatrixGraph::undirectedMatrixGraph(unsigned int n) : matrix(((n+1) * n) / 2),n(n) {}

undirectedMatrixGraph::undirectedMatrixGraph() {}

unsigned int undirectedMatrixGraph::getMatrixPosition(Vertex start, Vertex end)
{
	unsigned int point = 0;
	if(start < end)
		std::swap(start,end);
	unsigned int x = 0;
	while(x < start) { point += x++; }
	point += end;
	return point;
}

Edge undirectedMatrixGraph::getEdge(Vertex start, Vertex end)
{
	auto point = getMatrixPosition(start,end);
	Edge e(start,end,matrix[point]);
	return e;
}
bool undirectedMatrixGraph::setEdge(Vertex start, Vertex end, double weight)
{
	auto point =  getMatrixPosition(start,end);
	if(point < (((n+1)*n)/2)) {
		matrix[point] = weight;
		return true;
	}
	return false;
}

std::vector<Edge> undirectedMatrixGraph::getEdges(bool withoutZeroWeight)
{
	std::vector<Edge> edges(((n+1) * n) / 2 );
	Vertex start = 0;
	Vertex end = 0;
	unsigned int mod = 1;
	for (unsigned int i = 0; i < n; ++i) {
		if(i % mod == 0) {
			++mod;
			++start;
			end = 0;
		} else { ++end; }
		if(!withoutZeroWeight || matrix[i] != 0)
			edges[i] = Edge(start,end,matrix[i]);
	}
	return edges;
}

unsigned int undirectedMatrixGraph::getSize() { return n; }

std::vector<Vertex> undirectedMatrixGraph::getVertices()
{
	std::vector<Vertex> vertices(n);
	for (unsigned int i = 0; i < n; ++i) {
		vertices[i] = i;
	}
	return vertices;
}