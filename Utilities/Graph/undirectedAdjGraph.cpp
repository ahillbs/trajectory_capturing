//
// Created by gharg on 6/16/17.
//

#include <set>
#include <algorithm>
#include <iostream>
#include "undirectedAdjGraph.h"

//adjacency List only holds half of the Entries of an original Adjacency List

undirectedAdjGraph::undirectedAdjGraph() {}
undirectedAdjGraph::undirectedAdjGraph(unsigned int n) {
	for (unsigned int i = 0; i < n; ++i) {
		vertices.insert(i);
		adjList.insert(std::pair<Vertex,std::map<Vertex,double>>(i,std::map<Vertex,double>()));
	}
}

unsigned long undirectedAdjGraph::getSize() { return adjList.size(); }

const std::set<Vertex> & undirectedAdjGraph::getVertices() const{
	return this->vertices;
}

std::vector<Edge> undirectedAdjGraph::getEdges() const {
	ulong edgesDouble = 0;
	std::set<std::pair<Vertex ,Vertex >> edges;
	for (std::pair<Vertex,std::map<Vertex,double>> ad : adjList) {
		for(std::pair<Vertex,double> ad2 : adjList.at(ad.first)) {
			++edgesDouble;
			std::pair<Vertex ,Vertex > e = std::pair<Vertex ,Vertex >(ad.first,ad2.first);
			std::pair<Vertex ,Vertex > e2 = std::pair<Vertex ,Vertex >(ad2.first,ad.first);
			if((edges.find(e) == edges.end()) && (edges.find(e2) == edges.end())) {
				if(e.first < e.second)
					edges.insert(e);
				else
					edges.insert(e2);
			}
		}
	}
	//std::cout << "edges doubled: " << edgesDouble << std::endl;
	std::vector<Edge> vecEdges;
	for(std::pair<Vertex ,Vertex > e : edges) {
		vecEdges.push_back(this->getEdge(e.first,e.second).first);
	}
	std::sort(vecEdges.begin(),vecEdges.end());
	return vecEdges;
}

std::vector<Edge> undirectedAdjGraph::getEdges(Vertex v) const {
	std::vector<Edge> edges;
	if(adjList.find(v) != adjList.end()) {
		for (auto e : adjList.at(v)) {
			if(v < e.first)
				edges.push_back(Edge(v,e.first,e.second));
			else
				edges.push_back(Edge(e.first,v,e.second));
		}
	}
	return edges;

}

std::pair<Edge,bool> undirectedAdjGraph::getEdge(Vertex start, Vertex end) const{
	if(start > end)
		std::swap(start,end);
	bool exists = false;
	double weight = 0;
	if(adjList.find(start) != adjList.end()) {
		if(adjList.at(start).find(end) != adjList.at(start).end()) {
			weight = adjList.at(start).at(end);
			exists = true;
		}
	}
	return std::pair<Edge,bool>(Edge(start,end,weight),exists);
}
/// Adds Edge to adjacency List if there is no edge else sets new weight to the edge
/// \param start Vertex 1
/// \param end Vertex 2
/// \param weight weight for edge from vertex 1 to vertex 2
/// \return true if add/edit successful, false if vertex out of bounds
bool undirectedAdjGraph::setEdge(Vertex start, Vertex end, double weight) {
	if(start > end)
		std::swap(start,end);
	if(adjList.find(start) != adjList.end()) {
		if(adjList[start].find(end) != adjList[start].end()) {
			adjList[start][end] = weight;
			adjList[end][start] = weight;
		} else {
			adjList[start].insert(std::pair<Vertex, double>(end,weight));
			adjList[end].insert(std::pair<Vertex, double>(start,weight));
		}
		return true;
	}
	return false;
}
Vertex undirectedAdjGraph::addVertex() {
	Vertex newVertex = 0;
	if(adjList.size() > 0) {
		std::map<unsigned long, std::map<unsigned long, double>>::reverse_iterator lastVertexiterator = adjList.rbegin();
		newVertex = (lastVertexiterator->first + 1);
	}
	adjList.insert(std::pair<Vertex, std::map<Vertex, double>>(newVertex, std::map<Vertex, double>()));
	vertices.insert(newVertex);
	return newVertex;
}

bool undirectedAdjGraph::addVertex(Vertex v) {
	if(this->vertices.find(v) == this->vertices.end()) {
		this->adjList.insert(std::pair<Vertex, std::map<Vertex, double>>(v, std::map<Vertex, double>()));
		this->vertices.insert(v);
		return true;
	}
	return false;
}

void undirectedAdjGraph::clear() {
	vertices.clear();
	adjList.clear();
}

undirectedAdjGraph undirectedAdjGraph::createSubgraph(std::set<Vertex> &subGraphVertices) {
	undirectedAdjGraph subGraph;
	//Check if all vertices are in the graph, else error
	for (Vertex subVertex : subGraphVertices) {
		if(this->vertices.find(subVertex) == this->vertices.end()) {
			// a vertex was in the List
			throw std::invalid_argument("Some vertices are not in the original graph");
		}
		if(!subGraph.addVertex(subVertex)) {
			throw std::invalid_argument("Something went wrong while creating a vertex for subgraph");
		}
	}
	//Set edges for the subGraph
	for (Vertex subVertex : subGraphVertices) {
		for(auto edgePair : this->adjList[subVertex]) {
			if(subGraph.vertices.find(edgePair.first) != subGraph.vertices.end()) {
				subGraph.setEdge(subVertex,edgePair.first,edgePair.second);
			}
		}
	}

	return subGraph;
}

std::pair<Edge, bool> undirectedAdjGraph::getEdge(std::pair<Vertex, Vertex> &pair) {
	return getEdge(pair.first,pair.second);
}
