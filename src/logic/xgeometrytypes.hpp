#ifndef XGEOMETRYTYPES_HPP
#define XGEOMETRYTYPES_HPP
#pragma once

#include <type_traits>
#include <iostream>

template <class T>
struct point6D {
    static_assert(std::is_floating_point_v<T>, "Only for floating point numbers!");
    point6D(T _x, T _y, T _z, T _rx, T _ry, T _rz) :
        x(_x), y(_y), z(_z), rx(_rx), ry(_ry), rz(_rz) {};
    T x, y, z, rx, ry, rz;
    point6D<T> operator+(const point6D<T> &p) {return point6D<T>(x + p.x, y + p.y, z + p.z, rx + p.rx, ry + p.ry, rz + p.rz);}
    point6D<T> operator-(const point6D<T> &p) {return point6D<T>(x - p.x, y - p.y, z - p.z, rx - p.rx, ry - p.ry, rz - p.rz);}
    point6D<T> operator/(const double &div) {return point6D<T>(x / div, y / div, z / div, rx / div, ry / div, rz / div);}
    point6D<T> operator*(const double &mul) {return point6D<T>(x * mul, y * mul, z * mul, rx * mul, ry * mul, rz * mul);}
    bool operator==(const point6D<T> &p) {return (x == p.x && y == p.y && z == p.z && rx == p.x && ry == p.y && rz == p.z) ? true : false;}
    friend std::ostream& operator<<(std::ostream &os, const point6D &p) {return os << "(" << p.x << ", " << p.y << ", " << p.z
                                                                                   << ", " << p.rx << ", " << p.ry << ", " << p.rz << ")";}
};
template <class T>
struct point3D {
    static_assert(std::is_floating_point_v<T>, "Only for floating point numbers!");
    point3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};
    T x, y, z;
    point3D<T> operator+(const point3D<T> &p) {return point3D<T>(x + p.x, y + p.y, z + p.z);}
    point3D<T> operator-(const point3D<T> &p) {return point3D<T>(x - p.x, y - p.y, z - p.z);}
    point3D<T> operator/(const double &div) {return point3D<T>(x / div, y / div, z / div);}
    point3D<T> operator*(const double &mul) {return point3D<T>(x * mul, y * mul, z * mul);}
    bool operator==(const point3D<T> &p) {return (x == p.x && y == p.y) ? true : false;}
    friend std::ostream& operator<<(std::ostream &os, const point3D &p) {return os << "(" << p.x << ", "
                                                                                   << p.y << ", "
                                                                                   << p.z << ")";}
};
template <class T>
struct point2D {
    static_assert(std::is_floating_point_v<T>, "Only for floating point numbers!");
    point2D(T _x, T _y) : x(_x), y(_y) {};
    T x, y;
    point2D<T> operator+(const point2D<T> &p) {return point2D<T>(x + p.x, y + p.y);}
    point2D<T> operator-(const point2D<T> &p) {return point2D<T>(x - p.x, y - p.y);}
    point2D<T> operator/(const double &div) {return point2D<T>(x / div, y / div);}
    point2D<T> operator*(const double &mul) {return point2D<T>(x * mul, y * mul);}
    bool operator==(const point2D<T> &p) {return (x == p.x && y == p.y) ? true : false;}
    friend std::ostream& operator<<(std::ostream &os, const point2D &p) {return os << "(" << p.x << ", " << p.y << ")";}
};

#endif // XGEOMETRYTYPES_HPP
