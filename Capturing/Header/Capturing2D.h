//
// Created by Alexander Hill on 6/12/17.
//

#ifndef BA_CAPTURINGOBJECTS_H
#define BA_CAPTURINGOBJECTS_H


#include <vector>
#include "../../cplex/cplex.hpp"
//#include <ilconcert/iloexpression.h>
//#include <ilconcert/ilomodel.h>
//#include <ilcplex/ilocplexi.h>
#include "Segment.h"
#include "../../Utilities/Graph/undirectedAdjGraph.h"
#include "CapturingGraph.h"
#include "CapturingMetaData.h"
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Box_intersection_d/Box_with_handle_d.h>


using NT1 = CGAL::Quotient<CGAL::MP_Float>;
using Kernel = CGAL::Cartesian<NT1>;
typedef CGAL::Point_2<Kernel>                                 Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
typedef Traits_2::Segment_2                             Segment_2;
typedef std::vector<Segment_2>                               Segments;
typedef Segments::iterator                                   Iterator;
typedef CGAL::Box_intersection_d::Box_with_handle_d<double,2,Iterator> Box;


class Capturing2D : public CapturingGraph {
private:
	struct CGAL_CapturingReport {
		const Segments* segments1;
		std::vector<std::set<Point_2>>* familyPoints;
		std::map<Point_2,std::set<Vertex>>* vectorVertexMap;
		std::set<Point_2>* points;
		CGAL_CapturingReport(Segments *segments, std::vector<std::set<Point_2>>* familyPoints,
					std::map<Point_2,std::set<Vertex>>* vectorVertexMap, std::set<Point_2>* points)
				: segments1(segments), familyPoints(familyPoints) , vectorVertexMap(vectorVertexMap) , points(points)
		{}
		// callback functor that reports all truly intersecting segments
		void operator()(const Box* a, const Box* b) {
			//std::cout << "Box " << (a->handle() - segments1->begin()) << " and "
			//		  << (b->handle() - segments1->begin()) << " intersect";
			//std::cout << '.' << std::endl;
			Segment_2 &segA = *(a->handle());
			Segment_2 &segB = *(b->handle());

			CGAL::cpp11::result_of<Kernel::Intersect_2(Segment_2, Segment_2)>::type result = CGAL::intersection(segA,
																												segB);
			if(!result.is_initialized())
				return;

			 if (Point_2 *p = boost::get<Point_2>(&*result)) {
				auto itBoolPair = points->insert(*p);
				uint indexA = a->handle() - segments1->begin();
				uint indexB = b->handle() - segments1->begin();
				//std::cout << "indexA: " << indexA << " indexB: " << indexB << std::endl;
				(*familyPoints)[indexA].insert(*itBoolPair.first);
				(*familyPoints)[indexB].insert(*itBoolPair.first);
				if(vectorVertexMap->find(*itBoolPair.first) == vectorVertexMap->end())
					(*vectorVertexMap)[*itBoolPair.first] = std::set<Vertex>();
				(*vectorVertexMap)[*itBoolPair.first].insert(a->handle() - segments1->begin());
				(*vectorVertexMap)[*itBoolPair.first].insert(b->handle() - segments1->begin());
			} /*else if (const Segment_2 *s = boost::get<Segment_2>(&*result)) {

				 std::cout << "Segment: " << *s << std::endl;
				 std::cout << "indexA: " << a->handle() - segments1->begin() << ": " <<
						   segA << std::endl;

				 std::cout << " indexB: " << b->handle() - segments1->begin() << ": "
						   << segB << std::endl;

			 }*/
		}
	};



private:
	bool converted;
    undirectedAdjGraph segmentGraph;
	uint xBound;
	uint yBound;
	std::vector<Segment<NT>> segments;
    std::vector<std::vector<Vector2D>> vectorFamilies;
	std::map<Vector2D, Vertex> vertexMapping;
	std::map<Vertex , Vector2D> vectorMapping;
	std::map<Vector2D, std::set<Vertex>> vectorSegmentMapping;
	CapturingMetaData  metaData = CapturingMetaData("",0,0,"");

public:
    //Constructors
    Capturing2D(const std::string &name, const std::string &generator, ulong sharedEndpoints);
    Capturing2D(const std::string &name, std::vector<Segment<NT>> segments, uint xBound, uint yBound,
                const std::string &generator, ulong sharedEndpoints);
    Capturing2D(std::vector<Segment<NT>> segments, uint xBound, uint yBound,
                    const CapturingMetaData &_metaData);
    Capturing2D(std::vector<std::vector<Vector2D>> &families, uint xBound, uint yBound,
                             const CapturingMetaData &_metaData);

	bool isConverted() const;

    void setMetaData(const CapturingMetaData &metaData);

    const CapturingMetaData &getMetaData() const;

    const std::vector<std::vector<Vector2D>> &getVectorFamilies() const;

	undirectedAdjGraph
	computeConnectedSubgraph(std::map<Vector2D, Vertex> *vertexMappingSub, std::map<Vertex, Vector2D> *vectorMappingSub,
							 std::vector<std::vector<Vector2D>> *vectorFamiliesSub) const;

    undirectedAdjGraph getSolutionSubgraph(const std::set<Vertex> &greedySolution);

    const std::map<Vector2D, Vertex> &getVertexMapping() const;

    const std::map<Vertex, Vector2D> &getVectorMapping() const;

	const std::map<Vector2D, std::set<Vertex>> &getVectorSegmentMapping() const;

    const std::vector<Segment<NT>> &getSegments() const;

	void setBounds(uint xBound, uint yBound);

    void convertSegments2Graph();

	void printGraph();

	void printGraph(const char *cName);

	void printGraph(std::string docName);

	void printGraph(const char *cName, const CapturingSolution &solution);

	void printGraph(std::string docName, const CapturingSolution &solution);

	std::string to_string();

	std::string to_string(CapturingSolution solution);

private:

	void convertToCGALSegments(const std::vector<Segment<NT>> &segs, Segments *cgal_segments);

	void computeIntersectionPoints(Segments *segments, std::set<Point_2> *points,
								   std::vector<std::set<Point_2>> *familyPoints,
								   std::map<Point_2, std::set<Vertex>> *vectorVertexMap);


	void convertCGALToGraph(const Segments &segments, std::set<Point_2> *points,
							std::vector<std::set<Point_2>> *familyPoints,
							std::map<Point_2, std::set<Vertex>> *vectorVertexMap, std::set<Vector2D> *vecSet);

	void translateGraph(const std::set<Vector2D> &pSet);

	std::pair<std::pair<Vector2D, Vector2D>, NT>
	getFamilyMaxLength(ulong segment, std::vector<std::vector<Vector2D>> vectorFamiliesSub);

	void createMetaData(CapturingMetaData *data, const std::string &generatorName, ulong endpoints);
};


#endif //BA_CAPTURINGOBJECTS_H
