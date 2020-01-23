//
// Created by gharg on 19.09.17.
//

#ifndef PROJECT_GENERATOR_H
#define PROJECT_GENERATOR_H

#include "../TestRawPoints.hpp"

template<typename Problem>
class Generator {
	virtual Problem generateInstance(std::vector<TestRawPoints<double,2>>* points,ulong amount,ulong n,ulong k, ulong seedStart) = 0;
	virtual std::string printSettings() = 0;
};
#endif //PROJECT_GENERATOR_H
