//
// Created by gharg on 13.02.18.
//

#ifndef PROJECT_ALLEDGESIPSOLVER_H
#define PROJECT_ALLEDGESIPSOLVER_H


#include "../../Capturing/Header/Capturing2D.h"
#include "CplexSettings.h"

class AllEdgesIPSolver {
private:
	CplexSettings settings;
public:
	const CplexSettings &getSettings() const;

	void setSettings(const CplexSettings &settings);

	CapturingSolution solve(Capturing2D *instance, ulong maxK = 0);
};


#endif //PROJECT_ALLEDGESIPSOLVER_H
