//
// Created by gharg on 07.02.18.
//

#include <iostream>
#include "OuterSharedSegmentsVertices.h"
#include "../Header/CapturingGraph.h"

void OuterSharedSegmentsVertices::getNeighborhood(CapturingSolution &capSol, std::unordered_set<Vertex> *neighborhood) {
	CapturingGraph& graph = *(capSol.getGraph());

	// Go through all segments and add vertices, that are not between two chosen points for all segments but
	// are on an adjacent Segment to the chosen Vertices

	//std::cout << "Amount Segments: " << graph.getVertexFamilies()->size() << std::endl;

	//first get all adjacent segments
	std::map<Vertex,std::set<Vertex>>& verFamilyMapping = graph.getVertexInFamiliesMapping();
	std::unordered_set<Vertex> adjacentFamily;
	for(auto vertexFamilyPair : verFamilyMapping) {
		for (auto segmentIndex : vertexFamilyPair.second) {
			adjacentFamily.insert(segmentIndex);
		}
	}
	//std::cout << "Amount Segments adjacent: " << adjacentFamily.size() << std::endl;
	for (auto family : adjacentFamily) {
		auto& vertices = (*graph.getVertexFamilies())[family];
		// forward and backwards trough the segment to find all outer vertices (that wont have zero weight if taken)
		Vertex firstVertex = Vertex()-1;
		Vertex lastVertex = Vertex()-1;
		ulong forwardIndex = 0;
		ulong backwardIndex = vertices.size()-1;
		while ( forwardIndex < backwardIndex && (firstVertex == Vertex()-1 || lastVertex == Vertex()-1)) {
			if(firstVertex == Vertex()-1) {
				if(capSol.getVertexSolution().find(vertices[forwardIndex]) != capSol.getVertexSolution().end()) {
					firstVertex = vertices[forwardIndex];
				} else {
					neighborhood->insert(vertices[forwardIndex]);
				}
				++forwardIndex;
			}
			if(lastVertex == Vertex()-1) {
				if(capSol.getVertexSolution().find(vertices[backwardIndex]) != capSol.getVertexSolution().end()) {
					lastVertex = vertices[backwardIndex];
				} else {
					neighborhood->insert(vertices[backwardIndex]);
				}
				--backwardIndex;
			}
		}
	}
}
