#ifndef XMATH_H
#define XMATH_H
#pragma once

#include <array>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <tuple>

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


    //takes balloutput from findball and converts it into a 3d point in the fram of det robotbase
    static cv::Point3d ball_position_to_robotframe(std::tuple<bool, cv::Mat, cv::Point2f, float> data);
private:
    static double constexpr baseRadian = (-66) * (M_PI/180);

};

#endif // XMATH_H
