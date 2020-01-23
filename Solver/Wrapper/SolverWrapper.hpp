//
// Created by gharg on 19.09.17.
//

#ifndef PROJECT_SOLVERWRAPPER_H
#define PROJECT_SOLVERWRAPPER_H

#include "../../Capturing/Header/Capturing2D.h"

class Capturing2D_Random_solution {
private:
	ulong seed = 23564563425;
public:
	ulong getSeed() const {
		return seed;
	}

	void setSeed(ulong seed) {
		Capturing2D_Random_solution::seed = seed;
	}

	CapturingSolution solve(Capturing2D* p, ulong maxK) {
		p->convertSegments2Graph();
		ulong size = p->getGraph()->getSize();
		ulong k = maxK;
		std::set<Vertex> vertices;

		std::mt19937_64 random(seed);
		std::uniform_int_distribution<Vertex> distribution(0,size);
		while(vertices.size() < k) {
			vertices.insert(distribution(random));
		}

		return CapturingSolution(vertices,p);
	}
};
#endif //PROJECT_SOLVERWRAPPER_H
