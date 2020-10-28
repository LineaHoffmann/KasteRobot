#ifndef XMATH_H
#define XMATH_H

#include <array>
#include <math.h>

#include "xexceptions.hpp"

/**
 * This math lib is entirely header implemented
 * and contains no classes, only functions
 *
 * It throws errors of type x_err::error on issues
 **/

std::array<double, 3> distance3d_to_v0_xyAngle_czAngle (std::array<double, 3> distXYZ,
                                    std::array<double,2> vAngleLim,
                                    std::array<double,2> velLim) {
    double dxy = distXYZ[1] / distXYZ[0];
    double theta_xy = atan(dxy);    // XY plane Throwing angle [rad]
    double target_c = sqrt( pow(distXYZ[0], 2) + pow(distXYZ[1], 2) ); // XY plane distance to target

    // Lets try the lowest allowed angles first, and iterate
    // until an acceptable release velocity is achieved
    double g = 9.82; // Accelleration constant
    double theta_cz = vAngleLim[0];
    double num_steps = 100;
    double theta_cz_stepsize = (vAngleLim[1] - vAngleLim[0]) / num_steps;
    double v0 = 0;
    while (v0 < velLim[0] || v0 > velLim[1] || theta_cz >= vAngleLim[0] || theta_cz < vAngleLim[1]) {
        v0 = sqrt(- ( g * pow(target_c, 2) ) / ( 2 * (distXYZ[2] - tan( theta_cz )) * target_c ) ) / cos(theta_cz);
        theta_cz+=theta_cz_stepsize;
    }

    // If we got through with no result, theta_cz > vAngleLim[1] should be true
    if (theta_cz > vAngleLim[1]) throw x_err::error(x_err::what::MATH_NO_RESULT);

    // Result is returned as [v0, XY angle, CZ angle]
    // C is the plane formed by X and Y
    return std::array<double, 3>{v0, theta_xy, theta_cz};
}

#endif // XMATH_H
