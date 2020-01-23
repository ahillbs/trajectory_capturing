//
// Created by gharg on 6/12/17.
//

#ifndef BA_SEGMENTS_H
#define BA_SEGMENTS_H

#include <cmath>
#include "../../Utilities/GeoUtil.h"

template <typename T = double> class Segment {
private:
    Vector2D start;
    Vector2D end;

public:

	Segment<T> operator *(const T magnifier) {
		return(Segment(start*magnifier,end*magnifier));
	}

	Segment<T>() : start(0,0), end(0,0) {}
	Segment<T>(T sX, T sY, T eX, T eY) : start(sX, sY), end(eX, eY) {}
	Segment<T>(Vector2<T> v1, Vector2<T> v2) : start(v1),end(v2) {}

	const Vector2<T> & GetStartVector() const{
		return start;
	}

	const Vector2<T> &GetEndVector() const{
		return end;
	}

	const T GetLength() const{
		//return GeoUtil::getLengthVector2D(start,end);
		return start.x;
	}

	bool operator<(const Segment<T> &v) const {
		return start < v.start;
	}

	bool operator>(const Segment<T> &v) const {
		return start > v.start;
	}

};
using Segment2D = Segment<double>;

namespace GeoUtil {
	template <typename T>
	T getLengthVector2(const Vector2<T> &start, const Vector2<T> &end) {
		auto diffX = end.x - start.x;
		if(end.x < start.x)
			diffX = start.x - end.x;

		auto diffY = start.y - end.y;
		if(start.y < end.y)
			diffY = end.y - start.y;
		T quadDiffX = diffX*diffX;
		T quadDiffY = diffY*diffY;
		T addedDiff = quadDiffX + quadDiffY;
		double t = CGAL::to_double(addedDiff);
		auto result = CGAL::approximate_sqrt(t);
		T r = CGAL::to_double(result);
		return r;
	}
	template <typename T>
	Vector2<T> getSegmentDirection(Segment<T> &s) {

		return Vector2<T>(s.GetEndVector().x - s.GetStartVector().x, s.GetEndVector().y - s.GetStartVector().y);
	}
	template <typename T>
	std::pair<Vector2<T>, bool> getIntersectionPoint(Segment<T> seg1, Segment<T> seg2) {
		Vector2<T> s1 = seg1.GetStartVector();
		Vector2<T> s2 = seg2.GetStartVector();
		Vector2<T> e1 = seg1.GetEndVector();
		Vector2<T> e2 = seg2.GetEndVector();
		Vector2<T> rVec;
		T divide = (((s1.x - e1.x) * (s2.y - e2.y)) - ((s1.y - e1.y) * (s2.x - e2.x)));
		if(divide == 0) {
			return std::pair<Vector2<T>,bool>(rVec,false);
		}
		rVec.x = (((s1.x*e1.y - s1.y *  e1.x) * (s2.x - e2.x)) - ((s1.x - e1.x) * ( s2.x*e2.y - s2.y*e2.x))) / divide;
		rVec.y = (((s1.x*e1.y - s1.y *  e1.x) * (s2.y - e2.y)) - ((s1.y - e1.y) * ( s2.x*e2.y - s2.y*e2.x))) / divide;

		// test if intersection point is within both segments
		bool segmentIntersection =  (
				(
						( (s1.x <= rVec.x && rVec.x <= e1.x) || (s1.x >= rVec.x && rVec.x >= e1.x) ) &&
						( (s1.y <= rVec.y && rVec.y <= e1.y) || (s1.y >= rVec.y && rVec.y >= e1.y) ) ) &&
				( ( (s2.x <= rVec.x && rVec.x <= e2.x) || (s2.x >= rVec.x && rVec.x >= e2.x) ) &&
				  ( (s2.y <= rVec.y && rVec.y <= e2.y) || (s2.y >= rVec.y && rVec.y >= e2.y) )
				)
		);
		return std::pair<Vector2<T>,bool>(rVec,
										segmentIntersection);
	}
}

#endif //BA_SEGMENTS_H
