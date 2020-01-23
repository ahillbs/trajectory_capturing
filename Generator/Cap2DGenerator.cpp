//
// Created by gharg on 09.09.17.
//

#include <random>
#include <functional>
#include "Header/Cap2DGenerator.h"
#include "../Import/Imports.h"


std::vector<Vector2D> Cap2DGenerator::getRandomPoints(ulong amount, ulong xBound, ulong yBound, ulong seed) {

	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+1);

	auto dice_randX = std::bind(std::uniform_int_distribution<ulong>(0,xBound), random);
	auto dice_randY = std::bind(std::uniform_int_distribution<ulong>(0,yBound), randomY);
	std::vector<Vector2D> vectors;
	for (unsigned long i = 0; i < amount; ++i) {
		ulong coordinates[2];
		coordinates[0] = dice_randX();
		coordinates[1] = dice_randY();
		vectors.push_back(Vector2D(coordinates[0],coordinates[1]));
	}
	return vectors;
}

std::vector<NT> Cap2DGenerator::generateRandomPoints(uint amount, uint xBound, uint yBound, ulong seed) {

	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+1);

	auto dice_randX = std::bind(std::uniform_int_distribution<int>(0,xBound), random);
	auto dice_randY = std::bind(std::uniform_int_distribution<int>(0,yBound), randomY);
	std::vector<NT> vectors;
	for (unsigned long i = 0; i < amount; ++i) {
		NT coordinates[2];
		coordinates[0] = dice_randX();
		coordinates[1] = dice_randY();
		vectors.push_back(coordinates[0]);
		vectors.push_back(coordinates[1]);
	}
	return vectors;
}

Capturing2D
Cap2DGenerator::generateRandomSharedEndPointsInstance(const std::vector<Vector2D> &points, ulong segmentAmount,
													  ulong seed, const std::string &instanceName) {
	std::vector<Segment<NT>> segments;
	std::mt19937_64 randomS(seed+2);
	NT maxX = 0,maxY = 0;
	unsigned long maxEdges = points.size()*(points.size()-1) / 2;
	/*std::vector<Segment> allSegments;
	for (unsigned long j = 0; j < maxEdges; ++j) {
		for (unsigned long i = j+1; i < maxEdges; ++i) {
			allSegments.push_back(Segment(vectors[i], vectors[j]));
		}
	}*/

	if(segmentAmount < maxEdges) {
		auto dice_randS = std::bind(std::uniform_int_distribution<unsigned long>(0, points.size() - 1), randomS);

		using longPair = std::pair<unsigned long, unsigned long>;
		std::set<longPair> indexPairs;
		for (unsigned long j = 0; j < segmentAmount; ++j) {
			auto vecIndex1 = dice_randS();
			auto vecIndex2 = dice_randS();
			while(vecIndex1 == vecIndex2) {
				vecIndex2 = dice_randS();
			}
			longPair pair = longPair(vecIndex1,vecIndex2);
			if(indexPairs.find(pair) == indexPairs.end()) {
				if(maxX < points[vecIndex1].x || maxX < points[vecIndex2].x) {
					maxX = points[vecIndex1].x > points[vecIndex2].x ?  points[vecIndex1].x : points[vecIndex2].x;
				}
				if(maxY < points[vecIndex1].y || maxX < points[vecIndex2].y) {
					maxY = points[vecIndex1].y > points[vecIndex2].y ?  points[vecIndex1].y : points[vecIndex2].y;
				}
				longPair pair2 = longPair(vecIndex2,vecIndex1);
				indexPairs.insert(pair);
				indexPairs.insert(pair2);

				segments.push_back(Segment<NT>(points[vecIndex1], points[vecIndex2]));
			} else {

			}
		}
	} else {
		std::cout << "Amount of Segments exceed maximum Edges for " << points.size() << " points! " << std::endl;
		std::cout << "Complete graph will be build instead" << std::endl;
		for (unsigned long j = 0; j < maxEdges; ++j) {
			if(maxX < points[j].x ) {
				maxX = points[j].x;
			}
			if(maxY < points[j].y ) {
				maxY = points[j].y;
			}
			for (unsigned long i = j+1; i < maxEdges; ++i) {
				segments.push_back(Segment<NT>(points[i], points[j]));
			}
		}
	}

	std::string genName = "RandS";
	std::string name = instanceName;
	name += "_";
	name += genName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());

	return Capturing2D(name, segments, (uint) (CGAL::to_double(maxX) * 1.05), (uint) (CGAL::to_double(maxY) * 1.05),
					   genName, points.size());
}

Capturing2D Cap2DGenerator::generateRandomSegmentInstance(uint amount, uint xBound, uint yBound, ulong seed) {

	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+1);

	std::vector<Segment<NT>> segments;


	auto dice_randX = std::bind(std::uniform_int_distribution<int>(0,xBound), random);
	auto dice_randY = std::bind(std::uniform_int_distribution<int>(0,yBound), randomY);
	for (uint i = 0; i < amount; ++i) {
		int coordinates[4];
		for (int j = 0; j < 4; ++j) {

			if(j % 2 == 0) {
				coordinates[j] = dice_randX();
			} else {
				coordinates[j] = dice_randY();
			}
		}
		segments.push_back(Segment<NT>(coordinates[0],coordinates[1],coordinates[2],coordinates[3]));
	}
	std::string genName = "Random";
	std::string name = genName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());

	return Capturing2D(name, segments, xBound, yBound, genName, 0);
}

Capturing2D
Cap2DGenerator::generateProbabilisticInstance(const std::vector<Vector2D> &points, double probability, ulong seed,
											  const std::string &instanceName) {


	std::mt19937_64 random(seed);

	std::vector<Segment<NT>> segments;
	auto dice_rand = std::bind(std::uniform_int_distribution<int>(0,100), random);

	NT maxX = 0;
	NT maxY = 0;
	std::set<std::pair<Vector2D,Vector2D>> pointPair;
	for (uint i = 0; i < points.size(); ++i) {
		if(maxX < points[i].x) {
			maxX = points[i].x;
		}
		if(maxY < points[i].y) {
			maxY = points[i].y;
		}
		for (uint j = i+1; j < points.size(); ++j) {
			if(dice_rand() <= probability) {
				segments.push_back(Segment<NT>(points[i],points[j]));
				auto inserted = pointPair.insert(std::pair<Vector2D,Vector2D>(points[i],points[j]));
				if(!inserted.second) {
					std::cout << points[i] << " and " << points[j] << "was already inserted" << std::endl;
				}
				auto insertedR = pointPair.insert(std::pair<Vector2D,Vector2D>(points[j],points[i]));
				if(!insertedR.second) {
					std::cout << points[i] << " and " << points[j] << "was already inserted" << std::endl;
				}
			}
		}
	}

	std::string genName = "Prob";
	std::string name = instanceName;
	name += "_";
	name += genName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());

	return Capturing2D(name, segments, (uint) (CGAL::to_double(maxX) * 1.05), (uint) (CGAL::to_double(maxY) * 1.05),
					   genName, points.size());
}

Capturing2D*
Cap2DGenerator::generateProbabilisticInstanceRef(const std::vector<Vector2D> &points, double probability, ulong seed,
                                              const std::string &instanceName) {


    std::mt19937_64 random(seed);

    std::vector<Segment<NT>> segments;
    auto dice_rand = std::bind(std::uniform_int_distribution<int>(0,100), random);

    NT maxX = 0;
    NT maxY = 0;
    std::set<std::pair<Vector2D,Vector2D>> pointPair;
    for (uint i = 0; i < points.size(); ++i) {
        if(maxX < points[i].x) {
            maxX = points[i].x;
        }
        if(maxY < points[i].y) {
            maxY = points[i].y;
        }
        for (uint j = i+1; j < points.size(); ++j) {
            if(dice_rand() <= probability) {
                segments.push_back(Segment<NT>(points[i],points[j]));
                auto inserted = pointPair.insert(std::pair<Vector2D,Vector2D>(points[i],points[j]));
                if(!inserted.second) {
                    std::cout << points[i] << " and " << points[j] << "was already inserted" << std::endl;
                }
                auto insertedR = pointPair.insert(std::pair<Vector2D,Vector2D>(points[j],points[i]));
                if(!insertedR.second) {
                    std::cout << points[i] << " and " << points[j] << "was already inserted" << std::endl;
                }
            }
        }
    }

    std::string genName = "Prob";
    std::string name = instanceName;
    name += "_";
    name += genName;
    name += "_S";
    std::stringstream sstream;
    sstream << std::hex << seed;
    name += sstream.str();
    name += "_F";
    name += std::to_string(segments.size());

    return new Capturing2D(name, segments, (uint) (CGAL::to_double(maxX) * 1.05), (uint) (CGAL::to_double(maxY) * 1.05),
                       genName, points.size());
}



std::vector<Point_22> Cap2DGenerator::calcIntersectionPoints(Segments2& firstSegments,Segments2& intersectionSegments) {
	std::vector<Box2> boxes;

	for ( Iterator2 i = firstSegments.begin(); i != firstSegments.end(); ++i) {
		Box2 b = Box2(i->bbox(), i);
		boxes.push_back(b);
	}
	// Create the corresponding vector of pointers to bounding boxes
	std::vector<Box2 *> ptr;
	for(auto& i: boxes){ptr.push_back(&i);}
	//for ( std::vector<Box>::iterator i = boxes.begin(); i != boxes.end(); ++i)
	//	ptr.push_back(&*i);
	std::vector<Box2> boxes2;
	for ( Iterator2 i = intersectionSegments.begin(); i != intersectionSegments.end(); ++i) {
		Box2 b = Box2(i->bbox(), i);
		boxes2.push_back(b);
	}
	std::vector<Box2 *> ptr2;
	for (std::vector<Box2>::iterator i = boxes2.begin(); i != boxes2.end(); ++i) {
		ptr2.push_back(&(*i));
	}
	// Run the self intersection algorithm with all defaults on the
	// indirect pointers to bounding boxes. Avoids copying the boxes.
	std::vector<Point_22> points;
	std::ptrdiff_t cutoff = 50;

	CGAL::box_intersection_d( ptr.begin(), ptr.end(),ptr2.begin(),ptr2.end(), CGAL_Report(points),cutoff);
	return points;
}

Capturing2D
Cap2DGenerator::generateRandomAllNodesInstance(const std::vector<Vector2D> points, ulong segmentAmount, ulong seed,
											   const std::string &instanceName) {
	Segments2 segments;
	//translate points to CGAL points
	std::set<Point_22> points_2;
	Point_22 bBoxPointLower(std::numeric_limits<double>::max(),std::numeric_limits<double>::max());
	Point_22 bBoxPointUpper(std::numeric_limits<double>::min(),std::numeric_limits<double>::min());
	//Arrangement_2 arr;
	//Walk_pl pl(arr);
	for (uint l = 0; l < points.size(); ++l) {
		Point_22 p = Point_22(CGAL::to_double(points[l].x),CGAL::to_double(points[l].y));
		points_2.insert(p);
		if(bBoxPointLower.x() > p.x()) {
			bBoxPointLower = Point_22(p.x(),bBoxPointLower.y());
		}
		if(bBoxPointLower.y() > p.y()) {
			bBoxPointLower = Point_22(bBoxPointLower.x(),p.y());
		}
		if(bBoxPointUpper.x() < p.x()) {
			bBoxPointUpper = Point_22(p.x(),bBoxPointUpper.y());
		}
		if(bBoxPointUpper.y() < p.y()) {
			bBoxPointUpper = Point_22(bBoxPointUpper.x(),p.y());
		}

		//CGAL::insert_point(arr,p);
	}
	//std::cout << bBoxPointLower << " ... " << bBoxPointUpper << std::endl;
	//std::cout << CGAL::to_double(bBoxPointLower.x()) << " :.: " << CGAL::to_double(bBoxPointLower.y()) << std::endl;
	//std::cout << CGAL::to_double(bBoxPointUpper.x()) << " .:. " << CGAL::to_double(bBoxPointUpper.y()) << std::endl;


	NT2 maxX = 10, maxY = 10;
	//unsigned long maxEdges = points.size() * (points.size() - 1) / 2;
	//std::cout << segmentAmount << " segments will be generated" << std::endl;
	using longPair = std::pair<unsigned long, unsigned long>;
	std::set<longPair> indexPairs;
	for (unsigned long j = 0; j < segmentAmount; ++j) {
		//std::cout << "Segment nr.: " << j << std::endl;
		std::mt19937_64 randomS(seed + 2 + points_2.size() + j);
		auto dice_randS = std::bind(std::uniform_int_distribution<unsigned long>(0, points_2.size() - 1), randomS);
		auto vecIndex1 = dice_randS();
		auto vecIndex2 = dice_randS();
		while (vecIndex1 == vecIndex2) {
			vecIndex2 = dice_randS();
		}
		longPair pair = longPair(vecIndex1, vecIndex2);
		auto it1 = points_2.begin();//arr.vertices_begin();
		auto it2 = points_2.begin();//arr.vertices_begin();
		if (indexPairs.find(pair) == indexPairs.end()) {
			std::advance(it1 , vecIndex1);
			std::advance(it2 , vecIndex2);
			if (maxX < it1->x() || maxX < it2->x()) {
				maxX = it1->x() > it2->x() ? it1->x() : it2->x();
			}
			if (maxY < it1->y() || maxX < it2->y()) {
				maxY = it1->y() > it2->y() ? it1->y() : it2->y();
			}
			longPair pair2 = longPair(vecIndex2, vecIndex1);
			indexPairs.insert(pair);
			indexPairs.insert(pair2);
			Segment_22 loneSegment(*it1, *it2);
			//std::cout << "segment " << loneSegment << std::endl;
			//std::cout << "point 1: " << *it1 << " point 2: " << *it2 << std::endl;
			//Segments2 loneSeg = {Segment_22(it1->point(), it2->point())};
			//std::cout << "calc intersections" << std::endl;
			//CGAL::insert(arr,loneSegment,pl,)
			//std::vector<Point_22> interPoints =
			//		calcIntersectionPoints(segments,loneSeg);
			//CGAL::insert(arr,loneSegment,pl);
			//std::cout << "new segment " << loneSegment << std::endl;
			for (auto l = segments.begin(); l != segments.end(); ++l) {
				//std::cout << "get intersection " << *l << std::endl;
				auto result = CGAL::intersection(*l,loneSegment);
				if(result.is_initialized()) {
					if (const Point_22 *p = boost::get<Point_22>(&*result)) {
						if(p->x() >= bBoxPointLower.x() && p->y() >= bBoxPointLower.y() &&
						   p->x() <= bBoxPointUpper.x() && p->y() <= bBoxPointUpper.y()) {
							points_2.insert(*p);
						}
						//else
							//std::cout << "found vertex out of bounds" << *p << ":" << dx << " " << dy << std::endl;
					} else {
						//const Segment_2 *s = boost::get<Segment_2>(&*result);
						//std::cout << "found Segment: " << *s << std::endl;
					}
				}
			}
			//std::cout << "finished" << std::endl;
			/*for (auto it = interPoints.begin(); it != interPoints.end(); ++it) {
				if(points_2.find(*it) == points_2.end()) {
					std::cout << "added intersection point " << CGAL::to_double(it->x()) << ":" << CGAL::to_double(it->y()) << std::endl;
					points_2.insert(*it);
				}

			}*/
			segments.push_back(loneSegment);
		} else {

		}
	}
	double xBound = CGAL::to_double(maxX);
	double yBound = CGAL::to_double(maxY);
	xBound *= 1.05;
	yBound *= 1.05;
	//std::cout << "finished building segment graph " << segments.size() << std::endl;
	std::vector<Segment<NT>> realSegments;
	for (auto it = segments.begin(); it != segments.end(); ++it) {

		NT2 nx1 = it->source().x();
		NT2 nx2 = it->target().x();
		NT2 ny1 = it->source().y();
		NT2 ny2 = it->target().y();
		/*double x1 = CGAL::to_double(nx1);
		double x2 = CGAL::to_double(nx2);
		double y1 = CGAL::to_double(ny1);
		double y2 = CGAL::to_double(ny2);*/
		//std::cout << nx1 << " " << nx2 << " " << ny1 << " " << ny2 << std::endl;
		//std::cout << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;
		realSegments.push_back(Segment<NT>(nx1, ny1, nx2, ny2));
	}
	//std::cout << "finished translate segment graph " << realSegments.size() << std::endl;
	std::string genName = "RandA";
	std::string name = instanceName;
	name += "_";
	name += genName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());


	return Capturing2D(name, realSegments, (uint) (xBound), (uint) (yBound), genName, points.size());
}

Capturing2D
Cap2DGenerator::generate2DAxisAlignedInstance(ulong segmentAmount, ulong horAxisAmount, ulong vertAxisAmount,
											  ulong xBound, ulong yBound, ulong seed) {
	if(vertAxisAmount <= 0 || horAxisAmount <= 0 || segmentAmount <= 0) {
		throw std::invalid_argument("vertical, horizontal or segment amount are less than 1");
	}
	using AxisDataType = ulong;
	using axisIntersectionPair = std::pair<AxisDataType,AxisDataType >;
	std::map<AxisDataType,std::set<axisIntersectionPair >> xAxisSegments;
	std::map<AxisDataType,std::set<axisIntersectionPair >> yAxisSegments;
	std::vector<AxisDataType> xAxis;
	std::vector<AxisDataType> yAxis;
	std::vector<Segment<NT>> segments;
	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+seed + 68432);
	std::mt19937_64 horVertSegmentDecision(5*seed + 58443168);
	auto dice_randX = std::bind(std::uniform_int_distribution<AxisDataType>(0,xBound), random);
	auto dice_randY = std::bind(std::uniform_int_distribution<AxisDataType>(0,yBound), randomY);
	auto dice_randHV = std::bind(std::uniform_int_distribution<int>(0,1), horVertSegmentDecision);
	// Create horizontal and vertical axis
	for (ulong j = 0; j < horAxisAmount; ++j) {
		xAxis.push_back(dice_randX());
	}
	for (ulong k = 0; k < vertAxisAmount; ++k) {
		yAxis.push_back(dice_randY());
	}


	std::mt19937_64 vertSegmentDecision(4*seed + 45443168);
	std::mt19937_64 horSegmentDecision(8*seed + 8443668);
	std::uniform_int_distribution<ulong> vertDistribution(0,yAxis.size()-1);
	std::uniform_int_distribution<ulong> horDistribution(0,xAxis.size()-1);
	uint maxFailures = 1000;
	for (ulong l = 0; l < segmentAmount; ++l) {
		bool isXAxis = (bool)dice_randHV();
		if(isXAxis) {
			// take two horizontal axis and one vertical and place a segment in between
			// also check if segment overlap with another segment
			AxisDataType yAxisOne;
			AxisDataType yAxisTwo;
			AxisDataType xAxisOne;
			bool intersect;
			uint failures = 0;
			do {
				intersect = false;
				yAxisOne = xAxis[vertDistribution(vertSegmentDecision)];
				yAxisTwo = xAxis[vertDistribution(vertSegmentDecision)];
				if(yAxisOne > yAxisTwo)
					std::swap(yAxisOne,yAxisTwo);
				if(yAxisOne == yAxisTwo) {
					intersect = true;
					continue;
				}
				xAxisOne = xAxis[horDistribution(horSegmentDecision)];

				if(xAxisSegments.find(xAxisOne) != xAxisSegments.end()) {
					for(axisIntersectionPair yAxisPair : xAxisSegments.at(xAxisOne)) {
						if((yAxisPair.first <= yAxisOne && yAxisPair.second >= yAxisOne) || (yAxisPair.first <= yAxisTwo && yAxisPair.second >= yAxisTwo) ||
								(yAxisPair.first >= yAxisOne  && yAxisPair.second <= yAxisTwo)) {
							intersect = true;
							++failures;
						}
					}
				}
			} while (intersect && failures < maxFailures);
			if(failures < maxFailures) {
				//insert segment
				if(xAxisSegments.find(xAxisOne) == xAxisSegments.end()) {
					xAxisSegments.insert(std::pair<AxisDataType ,std::set<axisIntersectionPair >>(xAxisOne,std::set<axisIntersectionPair>()));
				}
				xAxisSegments.at(xAxisOne).insert(axisIntersectionPair(yAxisOne,yAxisTwo));
				segments.push_back(Segment<NT>(NT(xAxisOne),NT(yAxisOne), NT(xAxisOne),NT(yAxisTwo)));
			}
		} else {
			// take two vertical axis and one horizontal and place a segment in between
			// also check if segment overlap with another segment
			AxisDataType yAxisOne;
			AxisDataType xAxisOne;
			AxisDataType xAxisTwo;
			bool intersect;
			uint failures = 0;
			do {
				intersect = false;
				yAxisOne = xAxis[vertDistribution(vertSegmentDecision)];
				xAxisOne = xAxis[horDistribution(horSegmentDecision)];
				xAxisTwo = xAxis[horDistribution(horSegmentDecision)];
				if(xAxisOne > xAxisTwo) {
					std::swap(xAxisOne,xAxisTwo);
				}
				if(xAxisOne == xAxisTwo) {
					intersect = true;
					continue;
				}
				if(yAxisSegments.find(yAxisOne) != yAxisSegments.end()) {
					for(axisIntersectionPair xAxisPair : yAxisSegments.at(yAxisOne)) {
						if((xAxisPair.first <= xAxisOne && xAxisPair.second >= xAxisOne) || (xAxisPair.first <= xAxisTwo && xAxisPair.second >= xAxisTwo) ||
								(xAxisPair.first >= xAxisOne  && xAxisPair.second <= xAxisTwo)) {
							intersect = true;
							++failures;
						}
					}
				}
			} while (intersect && failures < maxFailures);
			if(failures < maxFailures) {
				//insert segment
				if(yAxisSegments.find(yAxisOne) == yAxisSegments.end()) {
					yAxisSegments.insert(std::pair<AxisDataType ,std::set<axisIntersectionPair >>(yAxisOne,std::set<axisIntersectionPair>()));
				}
				yAxisSegments.at(yAxisOne).insert(axisIntersectionPair(xAxisOne,xAxisTwo));

				segments.push_back(Segment<NT>(NT(xAxisOne),NT(yAxisOne), NT(xAxisTwo),NT(yAxisOne)));
			}
		}
	}
	std::cout << "Segment amount: " << segments.size() << std::endl;
	std::string generatorName = "TANOC";
	std::string name = generatorName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());
	return Capturing2D(name, segments, xBound, yBound, generatorName, 0);
}

Capturing2D
Cap2DGenerator::generate2DAxisAlignedOverlapInstance(ulong segmentAmount, ulong horAxisAmount, ulong vertAxisAmount,
											  ulong xBound, ulong yBound, ulong seed) {
	if(vertAxisAmount <= 0 || horAxisAmount <= 0 || segmentAmount <= 0) {
		throw std::invalid_argument("vertical, horizontal or segment amount are less than 1");
	}
	using AxisDataType = ulong;
	using axisIntersectionPair = std::pair<AxisDataType,AxisDataType >;
	std::map<AxisDataType,std::set<axisIntersectionPair >> xAxisSegments;
	std::map<AxisDataType,std::set<axisIntersectionPair >> yAxisSegments;
	std::vector<AxisDataType> xAxis;
	std::vector<AxisDataType> yAxis;
	std::vector<Segment<NT>> segments;
	std::mt19937_64 random(seed);
	std::mt19937_64 randomY(seed+seed + 81824);
	std::mt19937_64 horVertSegmentDecision(5*seed + 75164815);
	auto dice_randX = std::bind(std::uniform_int_distribution<AxisDataType>(0,xBound), random);
	auto dice_randY = std::bind(std::uniform_int_distribution<AxisDataType>(0,yBound), randomY);
	auto dice_randHV = std::bind(std::uniform_int_distribution<int>(0,1), horVertSegmentDecision);
	// Create horizontal and vertical axis
	for (ulong j = 0; j < horAxisAmount; ++j) {
		xAxis.push_back(dice_randX());
	}
	for (ulong k = 0; k < vertAxisAmount; ++k) {
		yAxis.push_back(dice_randY());
	}


	std::mt19937_64 vertSegmentDecision(4*seed + 1547179);
	std::mt19937_64 horSegmentDecision(8*seed + 54136942);
	std::uniform_int_distribution<ulong> vertDistribution(0,yAxis.size()-1);
	std::uniform_int_distribution<ulong> horDistribution(0,xAxis.size()-1);
	for (ulong l = 0; l < segmentAmount; ++l) {
		bool isXAxis = (bool)dice_randHV();
		if(isXAxis) {
			// take two horizontal axis and one vertical and place a segment in between
			// also check if segment overlap with another segment
			AxisDataType yAxisOne;
			AxisDataType yAxisTwo;
			AxisDataType xAxisOne;
			yAxisOne = xAxis[vertDistribution(vertSegmentDecision)];
			yAxisTwo = xAxis[vertDistribution(vertSegmentDecision)];
				if(yAxisOne > yAxisTwo)
					std::swap(yAxisOne,yAxisTwo);
			xAxisOne = xAxis[horDistribution(horSegmentDecision)];

			//insert segment
			if(xAxisSegments.find(xAxisOne) == xAxisSegments.end()) {
				xAxisSegments.insert(std::pair<AxisDataType ,std::set<axisIntersectionPair >>(xAxisOne,std::set<axisIntersectionPair>()));
			}
			xAxisSegments.at(xAxisOne).insert(axisIntersectionPair(yAxisOne,yAxisTwo));
			segments.push_back(Segment<NT>(NT(xAxisOne),NT(yAxisOne), NT(xAxisOne),NT(yAxisTwo)));

		} else {
			// take two vertical axis and one horizontal and place a segment in between
			// also check if segment overlap with another segment
			AxisDataType yAxisOne;
			AxisDataType xAxisOne;
			AxisDataType xAxisTwo;
			yAxisOne = xAxis[vertDistribution(vertSegmentDecision)];
			xAxisOne = xAxis[horDistribution(horSegmentDecision)];
			xAxisTwo = xAxis[horDistribution(horSegmentDecision)];
			if(xAxisOne > xAxisTwo) {
				std::swap(xAxisOne,xAxisTwo);
			}
			//insert segment
			if(yAxisSegments.find(yAxisOne) == yAxisSegments.end()) {
				yAxisSegments.insert(std::pair<AxisDataType ,std::set<axisIntersectionPair >>(yAxisOne,std::set<axisIntersectionPair>()));
			}
			yAxisSegments.at(yAxisOne).insert(axisIntersectionPair(xAxisOne,xAxisTwo));
			segments.push_back(Segment<NT>(NT(xAxisOne),NT(yAxisOne), NT(xAxisTwo),NT(yAxisOne)));

		}
	}
	std::cout << "Segment amount: " << segments.size() << std::endl;
	std::string genName = "TAOC";
	std::string name = genName;
	name += "_S";
	std::stringstream sstream;
	sstream << std::hex << seed;
	name += sstream.str();
	name += "_F";
	name += std::to_string(segments.size());
	return Capturing2D(name, segments, xBound, yBound, genName, 0);
}
