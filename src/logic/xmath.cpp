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

cv::Point3d xMath::ball_position_to_robotframe(std::tuple<bool, cv::Mat, cv::Point2f, float> data)
{
    if (std::get<0>(data)) {
        cv::Point3d pointRobot(std::get<2>(data).x, std::get<2>(data).y, std::get<3>(data));

     //       cos(-66)  -sin(-66)  0
     //       sin(-66)   cos(-66)  0 men da y akse skal flippes kan man tilføje (-) foran elementerne i denne række.
     //          0         0       1
            cv::Matx33d rotationMatrix(  cos(baseRadian), -sin(baseRadian), 0.0,
                                        -sin(baseRadian), -cos(baseRadian), 0.0,
                                                    0.0,               0.0, 1.0 );


            pointRobot = rotationMatrix * pointRobot; //calc new values for position in baseframe
            pointRobot /= 10;; // point starts as cm, but needs to be in M.
        return pointRobot;

    } else {
        return cv::Point3d(0,0,0);
    }
}
