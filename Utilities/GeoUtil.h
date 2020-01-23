//
// Created by gharg on 6/12/17.
//

#ifndef BA_GEOUTIL_H
#define BA_GEOUTIL_H


#include <utility>
#include <iostream>
#include <CGAL/MP_Float.h>
using NT = CGAL::Quotient<double>;
//(Quotient MP_Float too slow. Therefore Quotient double)
//Changes the quotient that is used in EVERY class for segments and Vector2D!
//MOST IMPORTANT USING!!!
template <typename T = double> struct Vector2 {
    T x = 0;
    T y = 0;
    Vector2<T>() : x(0),y(0) { }

    Vector2<T>(T parX, T parY) : x(parX),y(parY) { }

    Vector2 operator +(const Vector2 &v) {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator -(const Vector2 &v) {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator *(const Vector2 &v) {
        return Vector2(x * v.x, y * v.y);
    }

	Vector2 operator *(const T &v) {
		return Vector2(x * v, y * v);
	}

    Vector2 operator /(const Vector2 &v) {
        return Vector2(x / v.x, y / v.y);
    }

	bool operator <(const Vector2 &v) const {
		return (x < v.x) || ((x == v.x) && (y < v.y));
	}
	bool operator >(const Vector2 &v) const {
		return (x > v.x) || ((x == v.x) && (y > v.y));
	}
	bool operator ==(const Vector2 &v) const {
		return (x == v.x) && (y == v.y);
	}
	bool operator !=(const Vector2 &v) const {
		return (x != v.x) || (y != v.y);
	}
	bool operator >=(const Vector2 &v) const {
		return (x >= v.x) || ((x == v.x) && (y >= v.y));
	}
	bool operator <=(const Vector2 &v) const {
		return (x <= v.x) || ((x == v.x) && (y <= v.y));
	}

	//bool operator() (Vector2D w,Vector2D v) { return (w.x < v.x) || ((w.x == v.x) && (w.y < v.y));


};
template <typename T = double>
std::ostream& operator <<(std::ostream& stream, const Vector2<T> &v) {
	stream << v.x << "," << v.y;
	return stream;
}
template <typename T = double>
std::istream &operator>> (std::istream &istr, Vector2<T> &a)
{
	istr >> a.x >> "," >> a.x;
	return istr;
}
//old Vector2D is now template Vector2
//this is to use the old Vector2D as before
using Vector2D = Vector2<NT>;

/*struct VecCompare : public std::binary_function<Vector2D, Vector2D, bool>
{
	bool operator()(const Vector2D& lhs,const Vector2D& rhs) const
	{
		return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y < rhs.y));
	}
};*/

//#include "../Capturing/Segment.h"



#endif //BA_GEOUTIL_H
