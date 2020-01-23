//
// Created by gharg on 27.03.18.
//

#ifndef PROJECT_GREEDYSOLVER_H
#define PROJECT_GREEDYSOLVER_H


#include <zconf.h>
#include "../../Capturing/Header/Capturing2D.h"

class GreedySolver {
public:
	CapturingSolution solve(Capturing2D *instance, ulong maxK = 0, ulong startSegment = ulong()-1);

private:
	Vertex getMaxLengthVertex(Capturing2D *instance, ulong segmentIndex, Vertex otherVertex) const;

	std::pair<std::pair<Vector2D, Vector2D>, NT>
	getHeaviestFamily(const std::vector<std::vector<Vector2D>> &vectorFamiliesSub) const;

	std::pair<Vertex, NT> getBestVertex(Capturing2D *instance, std::set<Vertex> &greedySolution,
										const std::map<Vector2D, Vertex> &vertexMappingSub,
										const std::map<Vertex, Vector2D> &vectorMappingSub,
										const std::vector<std::vector<Vector2D>> &vectorFamiliesSub);
	std::pair<std::pair<Vector2D, Vector2D>, NT> getFamilyMaxLength(ulong segment, std::vector<std::vector<Vector2D>> vectorFamiliesSub);

	std::vector<Vertex> getDeg1Vertices(const undirectedAdjGraph &greedySubGraph) const;

	std::pair<ulong, Vertex>
	getOtherSolVertexInSegment(Capturing2D *instance, const std::set<Vertex> &greedySolution, Vertex v) const;

	std::set<Vertex> getPossibleVertices(Capturing2D *instance, const std::set<Vertex> &greedySolution,
										 const std::map<Vector2D, Vertex> &vertexMappingSub,
										 const std::map<Vertex, Vector2D> &vectorMappingSub,
										 const std::vector<std::vector<Vector2D>> &vectorFamiliesSub) const;
};


#endif //PROJECT_GREEDYSOLVER_H
