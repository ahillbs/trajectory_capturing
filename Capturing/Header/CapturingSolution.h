//
// Created by gharg on 27.08.17.
//

#ifndef PROJECT_CAPTURINGSOLUTION_H
#define PROJECT_CAPTURINGSOLUTION_H

class CapturingGraph;
class Neighborhood;
#include <set>
#include <random>
#include <ostream>
#include "../../Utilities/Graph/undirectedGraph.h"
#include "../../Utilities/Graph/undirectedAdjGraph.h"
//#include "../Neighborhoods/Neighborhood.h"
//#include "CapturingGraph.h"



class CapturingSolution {
private:
	std::set<Vertex> vertexSolution;

	std::vector<Edge> edgeSolution;
	double solutionValue;
	double upperValue;
	CapturingGraph *capGraph;
	bool isEdgeSolutionInvalid;
public:
	CapturingSolution(const std::set<Vertex> vertexSolution, CapturingGraph *capGraph);
	CapturingSolution(CapturingGraph *capGraph);
	CapturingSolution(std::set<Vertex> &vertexSolution, std::vector<Edge> &edgeSolution, double value,
					  CapturingGraph *capGraph);
	CapturingGraph* getGraph() const;

	const std::set<Vertex> &getVertexSolution() const;
	std::set<Vertex>& getVertexSolution();
	void setVertexSolution(const std::set<Vertex> &vertexSolution);
	const std::vector<Edge> & getEdgeSolution() const;
    const std::vector<Edge> & getEdgeSolution();
	void setEdgeSolution(const std::vector<Edge> &edgeSolution);
	double getSolutionValue() const;
	void setSolutionValue(double solutionValue);
	std::vector<CapturingSolution> getNeighbourhood(unsigned int k);
	double getUpperValue() const;
	void setUpperValue(double upperValue);

	CapturingSolution iteratedLocalSearch(bool localNeighbour);
	CapturingSolution iteratedLocalSearch(Neighborhood *neighborhood);

	void deleteVertex(Vertex v);
	void addVertex(Vertex v);

	void computeSolution(bool include_edge_solution, bool include_solution_value);

	CapturingSolution getRandomLocalNeighbour(ulong seed = 0,bool localSearch = true);
	CapturingSolution getRandomLocalNeighbour(ulong seed = 0, Neighborhood *neighborhood = nullptr);
	bool checkFeasible();
	CapturingSolution getBestNeighbourSolution(std::unordered_set<double> *tabuSet,bool localSearch = true);

private:
	void removeRandomVertex(CapturingSolution &randNeighbour, std::mt19937_64 &random,
							std::uniform_int_distribution<ulong> &distribution, Vertex &removedVertex) const;

	void
	buildRelatedVertices(const CapturingSolution &Neighbour, Vertex removedVertex, std::set<Vertex> *relatedVertices) const;

	void addRandRelVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
							 std::uniform_int_distribution<ulong> &distribution,
							 const std::set<Vertex> &relatedVertices) const;
	void addRandRelVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
							 std::uniform_int_distribution<ulong> &distribution,
							 const std::unordered_set<Vertex> &relatedVertices) const;

	void addRandVertToSol(CapturingSolution &randNeighbour, std::mt19937_64 &random,
						  std::uniform_int_distribution<ulong> &distribution) const;
};

static std::ostream& operator <<(std::ostream& stream, const CapturingSolution &sol) {
	stream << std::string("Vertices: ");
	for(auto v : sol.getVertexSolution())
		stream << std::to_string(v) << ",";
	stream << " value: " << sol.getSolutionValue();
	return stream;
}

#endif //PROJECT_CAPTURINGSOLUTION_H
