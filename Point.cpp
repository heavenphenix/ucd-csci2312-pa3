//
// By David Lam
// 03.30.16
// Point.cpp
// 

#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include "Point.h"
#include "Exceptions.h"
namespace Clustering {
    unsigned int Point::__idGen = 0;
    const char Point::POINT_VALUE_DELIM = ',';
    Point::Point(unsigned int dimensions) {
        if (dimensions == 0)
        throw ZeroDimensionsEx();
        __id = __idGen;
        ++__idGen;
        __dim = dimensions;
        __values = new double[__dim];
        for (int i = 0;
        i < __dim;
        ++i) {
            __values[i] = 0;
        }
    }
    Point::Point(unsigned int dimensions, double *array) {
        __id = __idGen;
        ++__idGen;
        __dim = dimensions;
        __values = new double[__dim];
        for (int i = 0;
        i < __dim;
        ++i) {
            __values[i] = array[i];
        }
    }
    Point::Point(const Point &origin) {
        __dim = origin.__dim;
        __values = new double[__dim];
        __id = origin.__id;
        for (int i = 0;
        i < __dim;
        ++i) {
            __values[i] = origin.__values[i];
        }
    }
    Point &Point::operator=(const Point &origin) {
        if (__dim != origin.__dim)
        throw DimensionalityMismatchEx(__dim, origin.__dim);
        if (this != &origin) {
            __dim = origin.__dim;
            __id = origin.__id;
            if (__values != nullptr) {
                delete[] __values;
            }
            __values = new double[__dim];
            for (int i = 0;
            i < __dim;
            ++i) {
                __values[i] = origin.__values[i];
            }
        }
        return *this;
    }
    Point::~Point() {
        delete [] __values;
    }
    void Point::rewindIdGen() {
        __idGen--;
    }
    int Point::getId() const {
        return __id;
    }
    unsigned int Point::getDims() const {
        return __dim;
    }
    void Point::setValue(unsigned int dim, double val) {
        if (dim >= __dim) {
            throw OutOfBoundsEx(__dim, dim);
        }
        __values[dim] = val;
    }
    double Point::getValue(unsigned int dim) const {
        if (dim >= __dim) {
            throw OutOfBoundsEx(__dim, dim);
        }
        return __values[dim];
    }
    double Point::distanceTo(const Point &compPoint) const {
        if (__dim != compPoint.__dim)
        throw DimensionalityMismatchEx(__dim, compPoint.__dim);
        double sumOfProducts = 0;
        for (int i = 0;
        i < __dim;
        ++i) {
            sumOfProducts += pow(compPoint.getValue(i) - getValue(i), 2);
        }
        return sqrt(sumOfProducts);
    }
    Point &Point::operator*=(double val) {
        for (int i = 0;
        i < __dim;
        ++i) {
            __values[i] *= val;
        }
        return *this;
    }
    Point &Point::operator/=(double val) {
        for (int i = 0;
        i < __dim;
        ++i) {
            __values[i] /= val;
        }
        return *this;
    }
    const Point Point::operator*(double val) const {
        Point p(*this);
        p *= val;
        return p;
    }
    const Point Point::operator/(double val) const {
        Point p(*this);
        p /= val;
        return p;
    }
    double &Point::operator[](unsigned int index) {
        if (index >= __dim)
        throw OutOfBoundsEx(__dim, index);
        return __values[index];
    }
    const double &Point::operator[](unsigned int index) const {
        if (index >= __dim)
        throw OutOfBoundsEx(__dim, index);
        return __values[index];
    }
    Point &operator+=(Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
        throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        for (unsigned int i = 0;
        i < lhs.__dim;
        ++i) {
            lhs[i] += rhs[i];
        }
        return lhs;
    }
    Point &operator-=(Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
        throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        for (unsigned int i = 0;
        i < lhs.__dim;
        ++i) {
            lhs[i] -= rhs[i];
        }
        return lhs;
    }
    const Point operator+(const Point &lhs, const Point &rhs) {
        Point p(lhs);
        p += rhs;
        return p;
    }
    const Point operator-(const Point &lhs, const Point &rhs) {
        Point p(lhs);
        p -= rhs;
        return p;
    }
    bool operator==(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
        throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        if (lhs.__id != rhs.__id)
        return false;
        for (int i = 0;
        i < std::max(lhs.__dim, rhs.__dim);
        ++i) {
            if (lhs.getValue(i) != rhs.getValue(i))
            return false;
        }
        return true;
    }
    bool operator!=(const Point &lhs, const Point &rhs) {
        return !(lhs == rhs);
    }
    bool operator<(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
        throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        for (unsigned int i = 0;
        i < lhs.__dim;
        ++i) {
            if (lhs.getValue(i) != rhs.getValue(i)) {
                return (lhs.getValue(i) < rhs.getValue(i));
            }
        }
        return false;
    }
    bool operator>(const Point &lhs, const Point &rhs) {
        return (rhs < lhs);
    }
    bool operator<=(const Point &lhs, const Point &rhs) {
        return !(lhs > rhs);
    }
    bool operator>=(const Point &lhs, const Point &rhs) {
        return !(lhs < rhs);
    }
    std::ostream &operator<<(std::ostream &out, const Point &p) {
        int i = 0;
        for ( ;
        i < p.__dim - 1;
        ++i)
        out << p.__values[i] << ", ";
        out << p.__values[i];
    }
    std::istream &operator>>(std::istream &in, Point &p) {
        std::string str;
        std::getline(in, str);
        unsigned int size = (unsigned int)std::count(str.begin(), str.end(), p.POINT_VALUE_DELIM) + 1;
        std::stringstream ss(str);
        if (p.getDims() != size) {
            throw DimensionalityMismatchEx(p.__dim, size);
        }
        int index = 0;
        while (!ss.eof()) {
            std::string svalue;
            getline(ss, svalue, ',');
            std::stringstream streamvalue(svalue);
            streamvalue >> p.__values[index];
            ++index;
        }
        return in;
    }
}
