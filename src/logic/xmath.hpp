#ifndef XMATH_H
#define XMATH_H
#pragma once

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <array>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <cmath>
#include <rw/math.hpp>

#include "wx/log.h"

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

    static rw::math::Rotation3D<> calculateTCPRotation(const rw::math::Vector3D<> &vect);

    static void calcThrow(std::vector<double> &q, double x, double y);


    //takes balloutput from findball and converts it into a 3d point in the frame of det robotbase
    static std::vector<double> ball_position_to_robotframe(std::tuple<bool, cv::Mat, cv::Point2f, float> data);

private:
    //-22.25 grader for at koordinatsæt passer til robot base
    static double constexpr baseRadian = (-22.25) * (M_PI/180);

    static inline double deg2rad(double deg){
        return deg * M_PI / 180.;
    };

};

#endif // XMATH_H
