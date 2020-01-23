//
// Created by gharg on 07.02.18.
//

#ifndef PROJECT_OUTERSHAREDSEGMENTSVERTICES_H
#define PROJECT_OUTERSHAREDSEGMENTSVERTICES_H


#include "Neighborhood.h"

class OuterSharedSegmentsVertices : public Neighborhood{
public:
	OuterSharedSegmentsVertices() {}
	void getNeighborhood(CapturingSolution &capSol, std::unordered_set<Vertex> *neighborhood);
};


#endif //PROJECT_OUTERSHAREDSEGMENTSVERTICES_H
