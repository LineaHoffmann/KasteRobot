#include "xmath.hpp"
#include <cmath>
#include <rw/math.hpp>


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

void xMath::calculateTCPRotation(std::vector<double> &position)
{
    double roll = 0;
    double pitch = 0;
    double yaw = 0;
    double radius = sqrt(position[0]*position[0]+position[1]*position[1]);

    if (radius > 0.10 && radius < 0.20){
            yaw = (0.1-radius)*(20/0.01);
    } else if (radius > 0.700 && radius < 0.850) {
            yaw = (radius-0.700) * (0.150/20);
    }

    if (radius > 0.10 && radius < 0.20){
        roll += atan2(position[1]*10,position[0]*10);
    }

    roll    *=  (M_PI/180);
    pitch   *=  (M_PI/180);
    yaw     *=  (M_PI/180);


    rw::math::RPY rpy(roll, pitch, yaw);

    rw::math::Rotation3Dd rotMatrix = rpy.toRotation3D();

    //decomposing x y and z rotations
    position[3] = -M_PI + atan2(rotMatrix(2,1), rotMatrix(2,2));
    position[4] = -(M_PI)+atan2(-rotMatrix(2,0), sqrt(rotMatrix(2,1)*rotMatrix(2,1) + rotMatrix(2,2)*rotMatrix(2,2)));
    position[5] = atan2(rotMatrix(1,0), rotMatrix(0,0));

    std::cout << rpy << " | " << radius << " | " << position[1] << " | " << position[0] << std::endl;
    std::cout << rotMatrix << std::endl;

}

std::vector<double> xMath::ball_position_to_robotframe(std::tuple<bool, cv::Mat, cv::Point2f, float> data)
{
    if (std::get<0>(data)) {
        cv::Point3d pointRobot(std::get<2>(data).x, std::get<2>(data).y, (std::get<3>(data) - 4.5)); //-3.5 is for the base z offset

     //       cos(-66)  -sin(-66)  0
     //       sin(-66)   cos(-66)  0 men da y akse skal flippes kan man tilføje (-) foran elementerne i denne række.
     //          0         0       1
            cv::Matx33d rotationMatrix(  cos(baseRadian), -sin(baseRadian), 0.0,
                                        -sin(baseRadian), -cos(baseRadian), 0.0,
                                                    0.0,               0.0, 1.0 );


            pointRobot = rotationMatrix * pointRobot; //calc new values for position in baseframe

            pointRobot /= 100; // point starts as cm, but needs to be in M.

            //convert to vector
            std::vector<double> pointAndRotation;
            pointAndRotation.push_back(pointRobot.x);
            pointAndRotation.push_back(pointRobot.y);
            pointAndRotation.push_back(pointRobot.z);
            pointAndRotation.push_back(0.720);
            pointAndRotation.push_back(-3.062);
            pointAndRotation.push_back(0.044);
//            pointAndRotation.push_back(1.778);
//            pointAndRotation.push_back(2.577);
//            pointAndRotation.push_back(-0.010);

            //calculateTCPRotation(pointAndRotation);

            std::stringstream s;
                        s << "x: " << pointAndRotation[0] << " | y: "
                          << pointAndRotation[1] << " | z: "
                          << pointAndRotation[2] << " || rx: "
                          << pointAndRotation[3] << " || ry: "
                          << pointAndRotation[4] << " || rz: "
                          << pointAndRotation[5];

                        logstd(s.str().c_str());

        return pointAndRotation;

    } else {
        return std::vector<double>{0,0,0,0,0,0};
    }
}
