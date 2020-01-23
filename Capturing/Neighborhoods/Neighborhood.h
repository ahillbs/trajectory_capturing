//
// Created by gharg on 07.02.18.
//

#ifndef PROJECT_NEIGHBORHOOD_H
#define PROJECT_NEIGHBORHOOD_H


#include <unordered_map>
#include <unordered_set>
#include "../Header/CapturingSolution.h"

class Neighborhood {
public:
	virtual void getNeighborhood(CapturingSolution &capSol, std::unordered_set<Vertex> *neighborhood) = 0;
	/*void testmethodInheritance() {
		CapturingGraph g;
		std::unordered_set s;
		getNeighborhood(CapturingSolution(g),&s);
	}*/
};


#endif //PROJECT_NEIGHBORHOOD_H
