#include "xmath.hpp"


std::array<double, 3> xMath::distance3d_to_v0_xyAngle_czAngle(const std::array<double, 3> &distanceXYZ,
                                                              const std::array<double, 2> &angleLim,
                                                              const std::array<double, 2> &velocityLim,
                                                              double steps,
                                                              double g)
{
    double dxy = distanceXYZ[1] / distanceXYZ[0];
    double theta_xy = atan(dxy);    // XY plane Throwing angle [rad]
    double target_c = sqrt( pow(distanceXYZ[0], 2) + pow(distanceXYZ[1], 2) ); // XY plane distance to target
    // Lets try the lowest allowed angles first, and iterate
    // until an acceptable release velocity is achieved
    double theta_cz = angleLim[0];
    double theta_cz_stepsize = (angleLim[1] - angleLim[0]) / steps;
    double v0 = 0;
    while (true) {
        // This small beast is from sweet Matlab
        v0 = ( pow( 2 , 0.5 ) * target_c * pow(( g / ( distanceXYZ[2] + tan( theta_cz ))), 0.5 )) / (2 * cos( theta_cz ));
        if ((v0 < velocityLim[0] || v0 > velocityLim[1]) && theta_cz < angleLim[1]) {
            theta_cz+=theta_cz_stepsize;
        } else {
            break;
        }
    }
    // If we got through with no result, theta_cz >= vAngleLim[1] should be true
    if (theta_cz >= angleLim[1]) throw x_err::error(x_err::what::MATH_NO_RESULT);
    return std::array<double,3>{v0, theta_xy, theta_cz};
}
