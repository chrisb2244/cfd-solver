/* ---------------------------------------------------------------------------
 * Vector class to be used for examining particular values or extracting
 * vectors at a point in a field, or alternatively holding positions in space
 *
 * Don't intend to use this for the data type of a Field, as would require the
 * construction of a huge number of Vector objects, which I expect would be
 * slow
 * --------------------------------------------------------------------------*/

#ifndef VECTOR_H
#define VECTOR_H

#include "TemplateFunctions.H"
#include <array>

template<typename T, size_t D>
class Vector
{
public:
    // Default constructor (needed?)
    Vector():
        value_(std::array<T,D>())
    {}

    // Construct from literal values
    template<typename... Vals, EnableIf<areT<T, Vals...>::value>..., EnableIf<sizeof...(Vals)==D>...>
    Vector(Vals... vals):
        value_(std::array<T,D>{vals...})
    {}

    template<size_t vecD = D, EnableIf<vecD>=1>...>
    T &x() { return value_[0]; }
    template<size_t vecD = D, EnableIf<vecD>=2>...>
    T &y() { return value_[1]; }
    template<size_t vecD = D, EnableIf<vecD>=3>...>
    T &z() { return value_[2]; }

private:
    std::array<T, D> value_;
};

template<typename T>
class Vector<T, 1>
{
public:
    Vector():
        value_()
    {}

    Vector(T val):
        value_(val)
    {}

    void operator=(const T &val) { value_ = val; }
    Vector<T, 1>& operator+=(const T &rhs) { value_ += rhs; return *this; }
    T &x() { return value_; } // To satisfy vectorField when mD == 1
private:
    T value_;
};


#endif // VECTOR_H
