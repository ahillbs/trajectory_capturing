//
// Created by root on 17.02.18.
//

#ifndef PROJECT_CPLEXSETTINGS_H
#define PROJECT_CPLEXSETTINGS_H

#include <zconf.h>
#include "../../Capturing/Header/CapturingSolution.h"

struct CplexSettings {
    ulong timeLimit = 900;
    bool output = false;
    bool relaxation = false;
    bool edgeRelaxation = false;
    CapturingSolution* presolution = nullptr;
    bool variableResultOutput = false;
    bool modelOutput = false;
};

#endif //PROJECT_CPLEXSETTINGS_H
