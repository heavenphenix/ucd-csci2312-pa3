//
// Created by Richard Au on 3/20/2016. Modified for PA3
//

#include <cstdlib>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "Cluster.h"
#include "Exceptions.h"

namespace Clustering{

    //gives sequential id's to clusters
    unsigned int Cluster::__idGenerator = 0;

    // node constructor
    LNode::LNode(const Point &p, LNodePtr n = nullptr) : point(p) {
        next = n;
    }

    //copy const for centroid
    Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) : __c(c), __p(Point(d)) {
        __dimensions = d;
        __valid = false;
    }

    const Point Cluster::Centroid::get() const{
        return __p;
    }

    //sets centroid to specific point
    void Cluster::Centroid::set(const Point &p){
        __p = p;
        __valid = true;
    }

    bool Cluster::Centroid::isValid() const{
        return __valid;
    }


    void Cluster::Centroid::setValid(bool valid){
        __valid = valid;
    }

    //computes the mean of all points of a cluster to determine a virtual centroid
    void Cluster::Centroid::compute() {
        double total = 0;
        Point tempP(__dimensions);
        if(__c.__points == nullptr)
            this->toInfinity();
        else {
            for (int point = 0; point < __c.__size; ++point)
                tempP += __c[point];
            for (int dim = 0; dim < __dimensions; ++dim)
                __p[dim] = tempP[dim] / __c.__size;
        }

        __valid = true;
    }

    //checks only values of points to see if equal
    bool Cluster::Centroid::equal(const Point &p) const{
        bool condition = true;

        for (unsigned int count = 0; count < __dimensions; ++count)
            if (__p.getValue(count) != p.getValue(count))
                condition = false;

        return condition;
    }

    //sets centroid to maximum amount double can hold
    void Cluster::Centroid::toInfinity(){
        for(int count = 0; count < __dimensions; ++count)
            __p[count] = std::numeric_limits<double>::max();
    }
    // cluster constructor
    Cluster::Cluster(unsigned int d) : centroid(d, *this){
        __dimensionality = d;
        __size = 0;
        __points = nullptr;
        __id = __idGenerator++;
    }

    // copy constr for cluster
    Cluster::Cluster(const Cluster &cluster) : centroid(cluster.__dimensionality, *this){
        __dimensionality = cluster.__dimensionality;
        __points = nullptr;
        __size = 0;
        for(int count = 0; count < cluster.__size; ++count) {
            add(cluster[count]);
        }
        __id = cluster.__id;
    }

//    void Cluster::__del() {
//        if (__points != nullptr) {
//            LNodePtr prevPtr = __points;
//            LNodePtr currPtr = nullptr;
//            while (prevPtr != nullptr) {
//                currPtr = prevPtr->next;
//                delete  prevPtr;
//                prevPtr = currPtr;
//            }
//        }
//        __points = nullptr;
//        __size = 0;
//    }

    // memb assignment
    Cluster &Cluster::operator=(const Cluster &cluster) {
        if(this == &cluster)
            return *this;
        else {
            if(__points != nullptr){
                LNodePtr currPtr = __points;
                LNodePtr nextPtr = nullptr;
                while(currPtr != nullptr){
                    nextPtr = currPtr->next;
                    delete currPtr;
                    currPtr = nextPtr;
                }
                __size = 0;
                __points = nullptr;
            }
            for(int count = 0; count < cluster.__size; ++count) {
                add(cluster[count]);
            }
        }
        __id = cluster.__id;
        return *this;
    }

    // postcondition: clears each node from linked list until no nodes are left and sets head pointer to null
    Cluster::~Cluster(){
        if(__points != nullptr){
            LNodePtr prevPtr = __points;
            LNodePtr currPtr = nullptr;
            while( prevPtr != nullptr){
                currPtr =  prevPtr->next;
                delete  prevPtr;
                prevPtr = currPtr;
            }
        }
        else
            assert(__size==0);
    }

    unsigned int Cluster::getSize() const {
        return __size;
    }

    unsigned int Cluster::getDimensionality() const{
        return __dimensionality;
    }

    unsigned int Cluster::getId() const{
        return __id;
    }

    //will use algorithm to determine efficient starting centroids
    void Cluster::pickCentroids(unsigned int k, Point **pointArray) {
        //if there are less than or equal points to k then set each centroid to the first k points of the cluster
        if(k >= __size){
            for(int count = 0; count < k; ++count){
                if(count < __size)
                    *pointArray[count] = (*this)[count];
                else
                    for(int i = 0; i < __dimensionality; ++i)
                        (*pointArray[count])[i] = std::numeric_limits<double>::max();
            }
        }
        else {

            //vectors to hold all points and centroid points
            std::vector<Point> kPoints;
            std::vector<Point> allPoints;
            allPoints.reserve(__size);
            kPoints.reserve(k);

            //initialize vector with every point
            for (int count = 0; count < __size; ++count)
                allPoints.push_back((*this)[count]);

            //initialize with first k points of all points
            for (int count = 0; count < k; ++count)
                kPoints.push_back(allPoints[count]);

            int vCount;
            int furthestIndex;

            //loop through each k point
            for (int kCount = 0; kCount < k; ++kCount) {
                furthestIndex = 0;
                //loop through all points and find the point farthest away from current point
                for (vCount = 1; vCount < allPoints.size(); ++vCount) {
                    if (kPoints[kCount].distanceTo(allPoints[furthestIndex]) > kPoints[kCount].distanceTo(allPoints[vCount]))
                        furthestIndex = vCount;
                }
                //set k point to furthest point available
                kPoints[kCount] = allPoints[furthestIndex];
                //remove from points so same point is not selected
                allPoints.erase(allPoints.begin() + furthestIndex);
            }
            for (int count = 0; count < k; ++count)
                *pointArray[count] = kPoints[count];
        }
    }

    // postcondition: point will be added to the cluster in pseudo-lexicographic order and size incremented
    void Cluster::add(const Point &point) {
        if(point.getDims() != __dimensionality)
            throw Clustering::DimensionalityMismatchEx(__dimensionality, point.getDims());
        Point p(point);
        LNodePtr insertPtr = new LNode(p, nullptr);
        LNodePtr prev = __points;
        LNodePtr next = __points;

        centroid.setValid(false);

        // if list is empty insert at "front"
        if(__points == nullptr) {
            __points = insertPtr;
            __size++;
        }

            // if only one item in list
        else if(__points->next == nullptr){

            // if passed point is less than existing, insert at head of list
            if (point < __points->point) {
                __points = insertPtr;
                insertPtr->next = prev;
                __size++;
            }
                // otherwise, insert at back
            else {
                __points->next = insertPtr;
                __size++;
            }
        }

            // 2 or more items in list
        else{
            next = next->next;

            // if passed point is less than first point, insert at head
            if(point < prev->point){
                __points = insertPtr;
                insertPtr->next = prev;
                __size++;
                return;
            }

            // else, find traverse list until find a point greater than passed point, then insert behind it
            while(next != nullptr){
                if(point < next->point) {
                    prev->next = insertPtr;
                    insertPtr->next = next;
                    __size++;
                    return;
                }
                next = next->next;
                prev = prev->next;
            }
            prev->next = insertPtr;
            __size++;
        }
    }

    // precondition: the point exists in the cluster
    // postcondition: the point will be removed from the cluster and returned from the function
    const Point &Cluster::remove(const Point &point) {
        if(point.getDims() != __dimensionality)
            throw Clustering::DimensionalityMismatchEx(__dimensionality, point.getDims());
        LNodePtr currPtr = __points;
        LNodePtr nextPtr = __points;

        // exit function if point doesn't exist
        if(__points == nullptr) {
            return point;
        }
            // if first point is equal to passed point, remove it
        else if(__points->point == point){
            __points = __points->next;
            delete currPtr;
            __size--;
        }

            // traverse list until point is found and remove it
        else{
            currPtr = __points;
            nextPtr = __points->next;
            while(nextPtr != nullptr){
                if(nextPtr->point == point){
                    currPtr->next = nextPtr->next;
                    delete nextPtr;
                    __size--;
                    break;
                }
                currPtr = nextPtr;
                nextPtr = nextPtr->next;
            }
        }

        centroid.setValid(false);
        return point;
    }

    // postcondition: returns true if point is found in cluster using Point::operator==, false otherwise
    bool Cluster::contains(const Point &point) const{
        LNodePtr cursor = __points;

        // traverse through list and only return true if point is found
        while(cursor != nullptr){
            if (cursor->point == point)
                return true;
            cursor = cursor->next;
        }
        return false;
    }

    // precondition: index passed is a valid index of the list
    // postcondition: returns point located at index of list
    const Point &Cluster::operator[](unsigned int index) const {
        if(__size == 0)
            throw Clustering::EmptyClusterEx();
        if(index < 0 || index >= __size)
            throw Clustering::OutOfBoundsEx(__size, index);
        else {
            LNodePtr cursor = __points;
            for (int count = 0; count < index; ++count)
                cursor = cursor->next;
            return cursor->point;
        }
    }

    // postcondition: inserts each point in list into ostream object
    std::ostream &operator<<(std::ostream &ostream1, const Cluster &cluster) {
        LNodePtr cursor = cluster.__points;
        for(int count = 0; count < cluster.__size; ++count){
            ostream1 << cursor->point;
            cursor = cursor->next;
            ostream1 << std::endl;
        }
        return ostream1;
    }

    // precondition: values contained in istream are comma seperated and each point is on new line
    // postcondition: extracts each point from istream and inserts into cluster
    std::istream &operator>>(std::istream &istream, Clustering::Cluster &cluster) {
        std::string line;
        while (getline(istream,line)) {
            // create point with correct dimension
            Point *pointPtr = new Clustering::Point(cluster.__dimensionality);
            std::stringstream lineStream(line);

            // call to Point::operator>>
            try {
                lineStream >> *pointPtr;
                cluster.add(*pointPtr);
            }
            catch (DimensionalityMismatchEx &ex) {
                pointPtr->rewindIdGen();
            }
            delete pointPtr;
        }
        return istream;
    }

    bool operator==(const Cluster &cluster1, const Cluster &cluster2) {
        if(cluster1.__dimensionality != cluster2.__dimensionality)
            throw Clustering::DimensionalityMismatchEx(cluster1.__dimensionality, cluster2.__dimensionality);
        if(cluster1.__size != cluster2.__size)
            return false;
        LNodePtr cursor1 = cluster1.__points;
        LNodePtr cursor2 = cluster2.__points;
        bool condition = true;

        while (cursor1 != nullptr && cursor2 != nullptr)
        {
            if(cursor1->point != cursor2->point)
                condition = false;
            cursor1 = cursor1->next;
            cursor2 = cursor2->next;
        }

        return condition;
    }

    bool operator!=(const Cluster &cluster, const Cluster &cluster1) {
        return(!(cluster == cluster1));
    }

    Cluster &Cluster::operator+=(const Point &point) {
        add(point);
        return *this;
    }

    Cluster &Cluster::operator-=(const Point &point) {
        remove(point);
        return *this;
    }

    Cluster &Cluster::operator+=(const Cluster &cluster) {
        if(__dimensionality != cluster.__dimensionality)
            throw Clustering::DimensionalityMismatchEx(__dimensionality, cluster.__dimensionality);
        for(int count = 0; count < cluster.__size; ++count){
            if(!contains(cluster[count]))
                add(cluster[count]);
        }
        return *this;
    }

    Cluster &Cluster::operator-=(const Cluster &cluster) {
        if(__dimensionality != cluster.__dimensionality)
            throw Clustering::DimensionalityMismatchEx(__dimensionality, cluster.__dimensionality);
        for(int count = 0; count < cluster.__size; ++count){
            if(contains(cluster[count]))
                remove(cluster[count]);
        }
        return *this;
    }

    const Cluster operator+(const Cluster &cluster, const Point &point) {
        Cluster c(cluster);
        return c += point;
    }

    const Cluster operator-(const Cluster &cluster, const Point &point) {
        Cluster c(cluster);
        return c -= point;
    }

    const Cluster operator+(const Cluster &cluster, const Cluster &cluster1) {
        Cluster c(cluster);
        return c += cluster1;
    }

    const Cluster operator-(const Cluster &cluster, const Cluster &cluster1) {
        Cluster c(cluster);
        return c -= cluster1;
    }

    Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to) : __p(p), __from(from), __to(to){ }

    void Cluster::Move::perform() {
        __to.add(__p);
        __from.remove(__p);
    }
}
