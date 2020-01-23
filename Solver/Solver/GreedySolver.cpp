//
// Created by gharg on 27.03.18.
//

#include "GreedySolver.h"

std::pair<std::pair<Vector2D, Vector2D>, NT>
GreedySolver::getHeaviestFamily( const std::vector<std::vector<Vector2D>> &vectorFamiliesSub) const {
	NT longestEdgeLength= 0;
	Vector2D longestVectorPair[2];
	for (unsigned long j = 0; j < vectorFamiliesSub.size(); ++j) {
		if(vectorFamiliesSub[j].size() > 1) {
			NT edgeLength = GeoUtil::getLengthVector2(vectorFamiliesSub[j][0],
													  vectorFamiliesSub[j][vectorFamiliesSub[j].size() - 1]);
			if (edgeLength > longestEdgeLength) {
				longestEdgeLength = edgeLength;

				longestVectorPair[0] = vectorFamiliesSub[j][0];
				longestVectorPair[1] = vectorFamiliesSub[j][vectorFamiliesSub[j].size() - 1];
			}
		}
	}
	return std::make_pair(std::make_pair(longestVectorPair[0],longestVectorPair[1]),longestEdgeLength);
}

Vertex GreedySolver::getMaxLengthVertex(Capturing2D *instance, ulong segmentIndex, Vertex otherVertex) const {
	Vertex maxLengthVector = Vertex() - 1;
	NT maxLength = 0;
	Vector2D pinnedVector = instance->getVectorMapping().at(otherVertex);
	for (Vector2D vec : instance->getVectorFamilies()[segmentIndex]) {

		NT length = GeoUtil::getLengthVector2(pinnedVector, vec);
		if (length > maxLength) {
			maxLength = length;
			maxLengthVector = instance->getVertexMapping().at(vec);
		}
	}
	return maxLengthVector;
}

std::pair<Vertex, NT>
GreedySolver::getBestVertex(Capturing2D *instance, std::set<Vertex> &greedySolution, const std::map<Vector2D, Vertex> &vertexMappingSub,
						   const std::map<Vertex, Vector2D> &vectorMappingSub,
						   const std::vector<std::vector<Vector2D>> &vectorFamiliesSub) {
	NT mostWeightGain= 0;
	Vertex mostWeightVertex= Vertex() - 1;
	std::set<Vertex> possibleVertices = getPossibleVertices(instance,greedySolution, vertexMappingSub, vectorMappingSub,
															vectorFamiliesSub);

	// search for most weight for all possible Vertices//std::map<unsigned long,std::pair<Vertex,Vertex>> newSegmentIndexesMap;
	for(Vertex v : possibleVertices) {
		NT weight = 0;
		//get all segment indexes that are adjacent to vertex v
		const std::set<unsigned long> &segmentIndexes = instance->getVectorSegmentMapping().at(vectorMappingSub.at(v));
		//std::map<unsigned long,std::pair<Vertex,Vertex>> possibleSegmentIndexesMap;
		for (unsigned long segIndex : segmentIndexes) {
			//when adjacent segment is already in solution there will be a weight gain
			NT innerMaxWeight = 0;
			NT existingSegmentWeight = 0;
			auto familyIndexPair = instance->getFamilyIndexPair(segIndex, greedySolution);
			if (familyIndexPair.second > familyIndexPair.first) {
				existingSegmentWeight = instance->getWeightInFamilyWithIndex(segIndex, familyIndexPair.first,
																   familyIndexPair.second);
			}
			for (auto vec : vectorFamiliesSub[segIndex]) {
				if (greedySolution.find(vertexMappingSub.at(vec)) != greedySolution.end()) {
					NT length = GeoUtil::getLengthVector2(vec, vectorMappingSub.at(v));
					if ((length - existingSegmentWeight) > innerMaxWeight) {
						innerMaxWeight = length;
					}
				}
			}
			weight += innerMaxWeight;
		}
		if (weight > mostWeightGain) {
			mostWeightGain = weight;
			mostWeightVertex = v;
		}
	}
	return std::make_pair(mostWeightVertex,mostWeightGain);
}

std::pair<std::pair<Vector2D, Vector2D>, NT> GreedySolver::getFamilyMaxLength(ulong segment, std::vector<std::vector<Vector2D>> vectorFamiliesSub) {
	NT longestEdgeLength = 0;
	Vector2D longestVectorPair[2];
	if (vectorFamiliesSub[segment].size() > 1) {
		NT edgeLength = GeoUtil::getLengthVector2(vectorFamiliesSub[segment][0],
												  vectorFamiliesSub[segment][vectorFamiliesSub[segment].size() -
																			 1]);
		longestEdgeLength = edgeLength;

		longestVectorPair[0] = vectorFamiliesSub[segment][0];
		longestVectorPair[1] = vectorFamiliesSub[segment][vectorFamiliesSub[segment].size() - 1];

		return std::make_pair(std::make_pair(longestVectorPair[0], longestVectorPair[1]), longestEdgeLength);
	} else {
		return std::make_pair(std::make_pair(Vector2D(0,0), Vector2D(0,0)), longestEdgeLength);
	}

}

std::set<Vertex> GreedySolver::getPossibleVertices(Capturing2D *instance, const std::set<Vertex> &greedySolution,
												  const std::map<Vector2D, Vertex> &vertexMappingSub,
												  const std::map<Vertex, Vector2D> &vectorMappingSub,
												  const std::vector<std::vector<Vector2D>> &vectorFamiliesSub) const {
	std::set<Vertex> possibleVertices;
	for(Vertex v : greedySolution) {
		for(Vertex segIndex : instance->getVectorSegmentMapping().at(vectorMappingSub.at(v))) {
			for (auto vec : vectorFamiliesSub.at(segIndex)) {
				possibleVertices.insert(vertexMappingSub.at(vec));
			}
		}
	}
	//delete already used vertices afterwards
	for(Vertex v : greedySolution) {
		possibleVertices.erase(v);
	}
	return possibleVertices;
}

std::vector<Vertex> GreedySolver::getDeg1Vertices(const undirectedAdjGraph &greedySubGraph) const {
	std::vector<Vertex> deg1Vertices;
	//look for Vertices with degree of 1
	for(Vertex v : greedySubGraph.getVertices()) {
		if(greedySubGraph.getEdges(v).size() <= 1) {
			deg1Vertices.push_back(v);
		}
	}
	return deg1Vertices;
}

std::pair<ulong, Vertex> GreedySolver::getOtherSolVertexInSegment(Capturing2D *instance, const std::set<Vertex> &greedySolution, Vertex v) const {
	Vertex otherVertex= Vertex() - 1;
	ulong segmentIndex= 0;
	NT lengthToOV = 0;
	for (unsigned long segIndex : instance->getVertexInFamiliesMapping().at(v)) {
		for (Vertex oV : (*instance->getVertexFamilies())[segIndex] ) {
			if (greedySolution.find(oV) != greedySolution.end()) {
				NT innerLength = GeoUtil::getLengthVector2(instance->getVectorMapping().at(v), instance->getVectorMapping().at(oV));
				if (innerLength > lengthToOV) {
					segmentIndex = segIndex;
					otherVertex = oV;
					lengthToOV = innerLength;
				}
			}
		}
	}
	return std::make_pair(segmentIndex,otherVertex);
}

CapturingSolution GreedySolver::solve(Capturing2D *instance, ulong maxK, ulong startSegment) {
	if(!instance->isConverted())
		instance->convertSegments2Graph();

	std::set<Vertex> greedySolution;
	// first we need to construct a subgraph where all vertices have a degree > 2
	std::map<Vector2D, Vertex> vertexMappingSub;
	std::map<Vertex , Vector2D> vectorMappingSub;
	std::vector<std::vector<Vector2D>> vectorFamiliesSub;
	undirectedAdjGraph subGraph = instance->computeConnectedSubgraph(&vertexMappingSub, &vectorMappingSub, &vectorFamiliesSub);

	std::pair<std::pair<Vector2D, Vector2D>, NT> vectorPair;
	if(startSegment != ulong() - 1) {
		vectorPair = getFamilyMaxLength(startSegment,vectorFamiliesSub);
		//if chosen segment will not have any points left after reduce chose best segment instead
		//just a fast workaround
		if (vectorPair.first.first == Vector2D(0,0) && vectorPair.first.second == Vector2D(0,0) && vectorPair.second == 0) {
			vectorPair = getHeaviestFamily(vectorFamiliesSub);
		}
	} else {
		// 1. search longest Segment
		vectorPair = getHeaviestFamily(vectorFamiliesSub);
	}
	std::cout << "heaviest segment found" << std::endl;
	greedySolution.insert(vertexMappingSub[vectorPair.first.first]);
	greedySolution.insert(vertexMappingSub[vectorPair.first.second]);
	NT currentWeight = vectorPair.second;

	// 2. loop which adds the vertex with most weight gain
	while(greedySolution.size() < maxK) {
		std::cout << "iterating..." << greedySolution.size() << std::endl;
		// sort out possible Vertices
		auto bestVertexPair = getBestVertex(instance,greedySolution, vertexMappingSub, vectorMappingSub, vectorFamiliesSub);

		//if no other connection is found there is no need to look further
		if(bestVertexPair.second == 0) {
			std::cout << "no weight gain. Get buffed, bro" << std::endl;
			break;
		}
		// add most weight Vertex to Solution
		greedySolution.insert(bestVertexPair.first);
		currentWeight += bestVertexPair.second;

	}
	// Vertices with degree of 1 in the greedy subgraph can be made longer without compromising the other Nodes
	// construct the subgraph with all nodes from/to solution nodes in a Segment
	undirectedAdjGraph greedySubGraph = instance->getSolutionSubgraph(greedySolution);

	std::vector<Vertex> deg1Vertices = getDeg1Vertices(greedySubGraph);
	// try to pin other Vertex in the same segment and augment length in original graph
	for(Vertex v : deg1Vertices) {
		//get other Vertex in the same segmentVertex otherVertex;
		auto segmentVertexPair = getOtherSolVertexInSegment(instance,greedySolution, v);
		if (segmentVertexPair.second != Vertex() - 1) {
			Vertex maxLengthVector = getMaxLengthVertex(instance,segmentVertexPair.first, segmentVertexPair.second);
			//augment segment
			if (greedySolution.find(maxLengthVector) != greedySolution.end()) {
				//std::cout << "something went wrong and it chose another solution vertex" << std::endl;
			} else {
				greedySolution.erase(v);
				greedySolution.insert(maxLengthVector);
			}
		}
	}

	return instance->computeSolution(greedySolution);
}


