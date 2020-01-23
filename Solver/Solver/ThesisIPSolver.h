//
// Created by root on 17.02.18.
//

#ifndef PROJECT_THESISIPSOLVER_H
#define PROJECT_THESISIPSOLVER_H


#include "CplexSettings.h"
#include "../../Capturing/Header/Capturing2D.h"

class ThesisIPSolver {
private:
    CplexSettings settings;
public:
    const CplexSettings &getSettings() const;

    void setSettings(const CplexSettings &settings);

    CapturingSolution solve(Capturing2D *instance, ulong k = 0);

};


#endif //PROJECT_THESISIPSOLVER_H
