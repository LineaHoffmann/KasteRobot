#ifndef XMATH_H
#define XMATH_H
#pragma once

#include <array>
#include <math.h>

#include "xexceptions.hpp"



/**
 * This math lib contains no classes, only functions
 * It throws errors of type x_err::error on issues
 **/

class xMath {
public:
    static std::array<double, 3> distance3d_to_v0_xyAngle_czAngle (const std::array<double,3> &distanceXYZ,
                                                                   const std::array<double,2> &angleLim,
                                                                   const std::array<double,2> &velocityLim,
                                                                   double steps = 10,
                                                                   double g = 9.82);
};

#endif // XMATH_H
