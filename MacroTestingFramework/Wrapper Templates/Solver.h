//
// Created by gharg on 19.09.17.
//

#ifndef PROJECT_SOLVER_H
#define PROJECT_SOLVER_H
template <typename Problem, typename Solution>
class Solver {
	virtual Solution solve(Problem* p) = 0;
};
#endif //PROJECT_SOLVER_H
