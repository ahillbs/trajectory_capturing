//
// Created by gharg on 18.09.17.
//

#ifndef PROJECT_TESTINSTANCE_H
#define PROJECT_TESTINSTANCE_H

#include <vector>
#include <string>
using uint = unsigned int;
using ulong = unsigned long;
template <typename point_type>
struct TestRawPoints {
	std::vector<std::vector<point_type>> points;
	ulong amount;
	std::string name;

public:
	TestRawPoints(std::string name, point_type *begin, point_type *end, ulong dimension) : points(dimension) {
		this->name = name;
		point_type* it = begin;

		//ulong internalAmount = (begin - end) / dimension;
		while(it < end) {
			for (uint i = 0; i < dimension; ++i) {
				points[i].push_back(*it);
				++it;
			}
		}
		this->amount = points[0].size();
	}
	ulong get_dimensions() {return points.size();}
	ulong get_point_amount() {return amount;}
	point_type getPoint(ulong index,uint pdimension) { return points[pdimension][index]; }
	void addPoint(std::vector<point_type> *point) {
		for (int i = 0; i < point->size(); ++i) {
			points[i].push_back((*point)[i]);
		}
	}

};

#endif //PROJECT_TESTINSTANCE_H
