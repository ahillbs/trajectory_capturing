//
// Created by gharg on 6/16/17.
//

#ifndef BA_UNDIRECTEDGRAPH_H
#define BA_UNDIRECTEDGRAPH_H

#include <vector>

using Vertex = unsigned long;
template <typename T, typename U, typename V> struct triplet {
	T first;
	U second;
	V third;
	triplet(T f, U s, V th) : first(f),second(s),third(th) {}
	triplet() {};
	bool operator > (const triplet<unsigned int, unsigned int, double> &triplet1) const {
		return (first > triplet1.first || (((first == triplet1.first) && ( second > triplet1.second))
										   || ((second == triplet1.second) && ( third > triplet1.third)) ) );
	};
	bool operator < (const triplet<unsigned int, unsigned int, double> &triplet1) const {
		return (first < triplet1.first || (((first == triplet1.first) && ( second < triplet1.second))
										   || ((second == triplet1.second) && ( third < triplet1.third)) ) );
	};
	// Example for implicit cast
	operator T() const {
		return first;

	}
};

using Edge = triplet<Vertex,Vertex,double>;

/*
class undirectedGraph {
public:
	undirectedGraph();
	undirectedGraph(unsigned int n);

	unsigned int getSize();
	std::vector<Vertex> getVertices();
	std::vector<Edge> getEdges(bool withoutZeroWeight);
	Edge getEdge(Vertex start, Vertex end);
	bool setEdge(Vertex start, Vertex end, double weight);
	Vertex addVertex();
};
*/


#endif //BA_UNDIRECTEDGRAPH_H
