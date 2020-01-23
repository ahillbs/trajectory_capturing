//
// Created by gharg on 26.09.17.
//

#ifndef PROJECT_TABUSEARCH_H
#define PROJECT_TABUSEARCH_H

#include "../../Capturing/Header/CapturingSolution.h"
#include "../../Capturing/Header/Capturing2D.h"

#ifndef BITSIZE
#define BITSIZE 64
#endif
namespace Metaheuristic {
	class TabuSearch {
		ulong memoryLength = 20000;
		ulong maxTime = 900;
	public:
		ulong getMaxTime() const;
		void setMaxTime(ulong maxTime);

		CapturingSolution solve(Capturing2D &problem, ulong maxK);
	private:
		void updateTabuList(const CapturingSolution &currSol, std::vector<double> &tabuList,
													   std::unordered_set<double> &tabuSet, ulong iteration);
	};
}

#endif //PROJECT_TABUSEARCH_H
