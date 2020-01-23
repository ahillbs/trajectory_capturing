//
// Created by gharg on 09.09.17.
//

#ifndef PROJECT_CAP2DGENERATOR_H
#define PROJECT_CAP2DGENERATOR_H


#include "../../Capturing/Header/Capturing2D.h"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Box_intersection_d/Box_with_handle_d.h>
#include <CGAL/box_intersection_d.h>
#include <CGAL/Arr_walk_along_line_point_location.h>
#include <CGAL/Arrangement_2.h>

using NT2			= CGAL::Quotient<double>;
using Kernel2		= CGAL::Cartesian<NT2>;
using Point_22		= CGAL::Point_2<Kernel2>;
using Traits_22		= CGAL::Arr_segment_traits_2<Kernel2>;
using Segment_22	= Traits_22::Segment_2;
//using Segment_22 	= Traits_22::X_monotone_curve_2;
using Segments2		= std::vector<Segment_22>;
using Iterator2		= Segments2::iterator;
using Box2			= CGAL::Box_intersection_d::Box_with_handle_d<double,2,Iterator2>;
using Arrangement_2	= CGAL::Arrangement_2<Traits_22>;

using Walk_pl 		= CGAL::Arr_walk_along_line_point_location<Arrangement_2>;

struct CGAL_Report {
	std::vector<Point_22>* points_2;
	CGAL_Report(std::vector<Point_22>& points)
			: points_2(&points)
	{}
	// callback functor that reports all truly intersecting segments
	void operator()(const Box2* a, const Box2* b) const {
		//std::cout << "Box " << (a->handle() - points_2->begin()) << " and "
		//		  << (b->handle() - points_2->begin()) << " intersect";
		//std::cout << '.' << std::endl;
		//if ( ! a->handle()->is_degenerate() && ! b->handle()->is_degenerate() &&
		//CGAL::Arr_segment_traits_2<CGAL::Cartesian<CGAL::Quotient<CGAL::MP_Float>>>::Segment_2
		Segment_22 &segA = *(a->handle());
		Segment_22 &segB = *(b->handle());



		auto result = CGAL::intersection(segA,segB);
		if(result.is_initialized()) {
			if (const Point_22 *p = boost::get<Point_22>(&*result)) {
				points_2->push_back(*p);
			} else {
				//const Segment_2 *s = boost::get<Segment_2>(&*result);
				//std::cout << "found Segment: " << *s << std::endl;
			}
		}
	}
};


class Cap2DGenerator {
public:
	static std::vector<Vector2D> getRandomPoints(ulong amount, ulong xBound, ulong yBound, ulong seed);

	static Capturing2D
	generateRandomSharedEndPointsInstance(const std::vector<Vector2D> &points, ulong segmentAmount,
                                              ulong seed, const std::string &instanceName);
	static Capturing2D generateRandomSegmentInstance(uint amount, uint xBound, uint yBound, ulong seed);
	static Capturing2D
	generateProbabilisticInstance(const std::vector<Vector2D> &points, double probability, ulong seed,
                                      const std::string &instanceName);
	static Capturing2D*
    generateProbabilisticInstanceRef(const std::vector<Vector2D> &points, double probability, ulong seed,
                                                     const std::string &instanceName);
	static Capturing2D
	generateRandomAllNodesInstance(const std::vector<Vector2D> points, ulong segmentAmount, ulong seed,
                                       const std::string &instanceName);
	static Capturing2D generate2DAxisAlignedInstance(ulong segmentAmount, ulong horAxisAmount, ulong vertAxisAmount,
													 ulong xBound, ulong yBound, ulong seed);
	static Capturing2D
	generate2DAxisAlignedOverlapInstance(ulong segmentAmount, ulong horAxisAmount, ulong vertAxisAmount, ulong xBound,
										 ulong yBound, ulong seed);

	static std::vector<NT> generateRandomPoints(uint amount, uint xBound, uint yBound, ulong seed);

private:

	static std::vector<Point_22> calcIntersectionPoints(Segments2& firstSegments,Segments2& intersectionSegments);
};


#endif //PROJECT_CAP2DGENERATOR_H
