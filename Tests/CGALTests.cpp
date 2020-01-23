//
// Created by gharg on 28.09.17.
//


#include "../Capturing/Header/Capturing2D.h"
#include "../Import/Imports.h"
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/box_intersection_d.h>
#include <list>
#include <gtest/gtest.h>

namespace {
using NT = CGAL::Lazy_exact_nt<CGAL::Quotient<CGAL::MP_Float>>;
	using Kernel = CGAL::Cartesian<NT>;
	typedef CGAL::Point_2<Kernel>                                 Point_2;
	typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
	typedef Traits_2::Segment_2                             Segment_2;
	typedef std::vector<Segment_2>                               Segments;
	typedef Segments::iterator                                   Iterator;
	typedef CGAL::Box_intersection_d::Box_with_handle_d<double,2,Iterator> Box;

	TEST(CGALTest,intersectionPointsTest) {
		std::cout << std::endl;
		std::cout << "Test bind in loop: " << std::endl;
		for (uint j = 50; j < 70; ++j) {
			std::mt19937_64 randomS(0 + 7 + j*3 );
			auto dice_randS = std::bind(std::uniform_int_distribution<unsigned long>(0, j), randomS);
			std::cout << dice_randS() << std::endl;
		}
		std::cout << "Test bind out of loop: " << std::endl;
		std::mt19937_64 randomT(0 + 7);
		auto dice_randS = std::bind(std::uniform_int_distribution<unsigned long>(0, 55), randomT);
		for (int j = 50; j < 70; ++j) {
			std::cout << dice_randS() << std::endl;
		}



		// Construct the input segments.
		Segment_2 segments[] = {Segment_2 (Point_2 (1, 5), Point_2 (8, 5)),
								Segment_2 (Point_2 (1, 1), Point_2 (8, 8)),
								Segment_2 (Point_2 (3, 1), Point_2 (3, 8)),
								Segment_2 (Point_2 (8, 5), Point_2 (8, 8))};



		// Compute all intersection points.
		std::list<Point_2>     pts;
		CGAL::compute_intersection_points (segments, segments + 4,
										   std::back_inserter (pts));
		std::set<Point_2> testPoints = {Point_2(3,3)};
		for (auto i = pts.begin(); i != pts.end(); ++i) {
			testPoints.insert(*i);
			std::cout << i->x() << " | " << i->y() << std::endl;
		}
		std::cout << "Points in set: " << std::endl;
		for (auto it = testPoints.begin(); it != testPoints.end(); ++it) {
			std::cout << it->x() << " | " << it->y() << std::endl;
		}

		Point_2 p(3,3);
		Vector2<NT> pt(p.x(),p.y());
		Vector2<NT> pt2(pts.begin()->x(),pts.begin()->y());


		//probably needed
		std::cout << "vector2 1 test " << pt.x << "," << pt.y << std::endl;
		std::cout << "vector2 2 test " << pt2.x << "," << pt2.y << std::endl;
		//CGAL_assertion(p == pts[0]);
		std::cout << "p and pts[0] are " << (pt >= pt2) << std::endl;
		// Print the result.
		std::cout << "Found " << pts.size() << " intersection points: " << std::endl;
		std::copy (pts.begin(), pts.end(),
				   std::ostream_iterator<Point_2>(std::cout, "\n"));

		// Compute the non-intersecting sub-segments induced by the input segments.
		std::list<Segment_2>   sub_segs;
		CGAL::compute_subcurves(segments, segments + 4, std::back_inserter(sub_segs));
		std::cout << "Found " << sub_segs.size()
				  << " interior-disjoint sub-segments." << std::endl;
		//CGAL_assertion (CGAL::do_curves_intersect (segments, segments + 4));
	}
	typedef CGAL::Quotient<int>                                     Number_type;
	typedef CGAL::Cartesian<Number_type>                            Kernel2;
	typedef CGAL::Arr_segment_traits_2<Kernel2>                     Traits_22;
	typedef Traits_22::Point_2                                       Point_22;
	typedef Traits_22::X_monotone_curve_2                            Segment_22;
	typedef CGAL::Arrangement_2<Traits_22>                           Arrangement_2;

	typedef CGAL::Arr_walk_along_line_point_location<Arrangement_2> Walk_pl;


	TEST(CGALTest,incrementalInsertion) {
		// Construct the arrangement of five intersecting segments.
		Arrangement_2  arr;
		Walk_pl        pl(arr);
		Segment_22      s1(Point_22(1, 0), Point_22(2, 4));
		Segment_22      s2(Point_22(5, 0), Point_22(5, 5));
		Segment_22      s3(Point_22(1, 0), Point_22(5, 3));
		Segment_22      s4(Point_22(0, 2), Point_22(6, 0));
		Segment_22      s5(Point_22(3, 0), Point_22(5, 5));
		insert_non_intersecting_curve(arr, s1, pl);
		insert_non_intersecting_curve(arr, s2, pl);
		insert(arr, s3, pl);
		insert(arr, s4, pl);
		insert(arr, s5, pl);
		// Print the size of the arrangement.
		std::cout << "The arrangement size:" << std::endl
				  << "   V = " << arr.number_of_vertices()
				  << ",  E = " << arr.number_of_edges()
				  << ",  F = " << arr.number_of_faces() << std::endl;
		// Perform a point-location query on the resulting arrangement and print
		// the boundary of the face that contains it.
		Point_22 q(4, 1);
		Walk_pl::result_type obj = pl.locate(q);

		Arrangement_2::Face_const_handle  f;
		CGAL_assertion_code(bool success =) CGAL::assign(f, obj);
		CGAL_assertion(success);
		std::cout << "The query point (" << q << ") is located in: ";
		//print_face<Arrangement_2>(f);

		auto iterator = arr.vertices_begin();
		while(iterator != arr.vertices_end()) {
			iterator->point().x();
			++iterator;
		}
	}

	struct CGAL_TestReport {
		Segments* point_2;
		CGAL_TestReport(Segments& triangles)
				: point_2(&triangles)
		{}
		// callback functor that reports all truly intersecting segments
		void operator()(const Box* a, const Box* b) const {
			std::cout << "Box " << (a->handle() - point_2->begin()) << " and "
					  << (b->handle() - point_2->begin()) << " intersect";
			std::cout << '.' << std::endl;
			//if ( ! a->handle()->is_degenerate() && ! b->handle()->is_degenerate() &&
			//CGAL::Arr_segment_traits_2<CGAL::Cartesian<CGAL::Quotient<CGAL::MP_Float>>>::Segment_2
			Segment_2 &segA = *(a->handle());
			Segment_2 &segB = *(b->handle());

			CGAL::cpp11::result_of<Kernel::Intersect_2(Segment_2, Segment_2)>::type result = CGAL::intersection(segA,
																												segB);
			if (const Segment_2 *s = boost::get<Segment_2>(&*result)) {
				std::cout << "Segment: " << *s << std::endl;
			} else {
				const Point_2 *p = boost::get<Point_2>(&*result);
				std::cout << "Point: " << *p << std::endl;
			}
		}
	};

	TEST(CGALTest, intersectionWithPointers) {
		std::cout << std::endl;
		Segments segments = {Segment_2 (Point_2 (1, 5), Point_2 (8, 5)),
								Segment_2 (Point_2 (1, 1), Point_2 (8, 8)),
							 	Segment_2 (Point_2 (3, 1), Point_2 (3, 8))

								};
		Segments lonelySegment = {
								   Segment_2 (Point_2 (8, 5), Point_2 (8, 8))
								   };
		//while(in >> t){
		//	segments.push_back(t);
		//}
		// Create the corresponding vector of bounding boxes
		std::vector<Box> boxes;
		for ( Iterator i = segments.begin(); i != segments.end(); ++i) {
			Box b = Box(i->bbox(), i);
			boxes.push_back(b);
		}

		// Create the corresponding vector of pointers to bounding boxes
		std::vector<Box *> ptr;
		for ( std::vector<Box>::iterator i = boxes.begin(); i != boxes.end(); ++i)
			ptr.push_back(&*i);

		std::vector<Box> boxes2;
		for ( Iterator i = lonelySegment.begin(); i != lonelySegment.end(); ++i) {
			Box b = Box(i->bbox(), i);
			boxes2.push_back(b);
		}

		std::vector<Box *> ptr2;
		for (std::vector<Box>::iterator i = boxes2.begin(); i != boxes2.end(); ++i) {
			ptr2.push_back(&(*i));
		}
		// Run the self intersection algorithm with all defaults on the
		// indirect pointers to bounding boxes. Avoids copying the boxes.
		CGAL::box_intersection_d( ptr.begin(), ptr.end(),ptr2.begin(),ptr2.end(), CGAL_TestReport(segments));
		//CGAL::box_self_intersection_d( ptr.begin(), ptr.end(), CGAL_Report(segments));
	}
}