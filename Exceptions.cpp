//
// By David Lam
// 03.30.16
// Exceptions.cpp
//

#include <cassert>
#include <sstream>

#include "Exceptions.h"
#include "Cluster.h"

using namespace std;
namespace Clustering {
    OutOfBoundsEx::OutOfBoundsEx(unsigned int c, int r) {
        this -> __current = c;
        this -> __rhs = r;
        __name = "Out Of Bounds Ex: ";
    }
    unsigned int OutOfBoundsEx::getCurrent() const{
        return this -> __current;
    }
    int OutOfBoundsEx::getRhs() const{
        return this -> __rhs;
    }
    string OutOfBoundsEx::getName() const{
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const OutOfBoundsEx &ex) {
        os << ex.getName() << " (current = " << ex.getCurrent() << ", rhs = " << ex.getRhs() << ')';
        return os;
    }
    DimensionalityMismatchEx::DimensionalityMismatchEx(unsigned int c, unsigned int r){
        this -> __current = c;
        this -> __rhs = r;
    }
    unsigned int DimensionalityMismatchEx::getCurrent() const{
        return this -> __current;
    }
    unsigned int DimensionalityMismatchEx::getRhs() const{
        return this -> __rhs;
    }
    string DimensionalityMismatchEx::getName() const{
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const DimensionalityMismatchEx &ex){
        os << "Dimensionality MisMatch Ex" << " ( current = " << ex.__current << "," << "rhs = " << ex.__rhs << ')';
    }
    ZeroClustersEx::ZeroClustersEx(){
        __name = "Zero Clusters Ex: ";
    }
    string ZeroClustersEx::getName() const{
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const ZeroClustersEx &ex){
        os << ex.getName();
        return os;
    }
    DataFileOpenEx::DataFileOpenEx(string filename) {
        this-> __filename = filename;
    }
    string DataFileOpenEx::getFilename() const{
        return this -> __filename;
    }
    string DataFileOpenEx::getName() const {
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const DataFileOpenEx &ex){
        os << ex.getName() << ", file name is " << ex.getFilename();
        return os;
    }
    ZeroDimensionsEx::ZeroDimensionsEx(){
        __name = "Zero Dimensions Ex: ";
    }
    string ZeroDimensionsEx::getName() const {
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const ZeroDimensionsEx &ex){
        os << ex.getName();
        return os;
    }
    EmptyClusterEx::EmptyClusterEx(){
        __name = "Empty Cluster Ex: ";
    }
    string EmptyClusterEx::getName() const {
        return this -> __name;
    }
    ostream &operator<<(ostream &os, const EmptyClusterEx &ex){
        os << ex.getName();
        return os;
    }
}
