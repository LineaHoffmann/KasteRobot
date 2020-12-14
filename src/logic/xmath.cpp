#include "xmath.hpp"



std::array<double, 3> xMath::distance3d_to_v0_xyAngle_czAngle(const std::array<double, 3> &distanceXYZ,
                                                              const std::array<double, 2> &angleLim,
                                                              const std::array<double, 2> &velocityLim,
                                                              double steps,
                                                              double g)
{
    double theta_xy = std::atan2(distanceXYZ[1], distanceXYZ[0]);    // XY plane Throwing angle [rad]
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

rw::math::Rotation3D<> xMath::calculateTCPRotation(const rw::math::Vector3D<> &vect)
{

    double thetax{0}, thetay{-180}, thetaz{0}, radius;

    radius = 10 * sqrt(pow(vect[0],2)+pow(vect[1],2));

    if (radius > 700 && radius < 950){
        double a = 35./(950-700);
        thetax = (radius-700)*a;
    } else if (radius > 100 && radius < 300){
        double a = 35./(300-100);
        thetax = -(300-radius)*a;
        thetay = (300-radius)*a;
    }

    thetaz = atan2(vect[1], vect[0]);
//    if(radius > 100 && radius < 300){
//        thetaz = atan2(vect[1], vect[0]);
//    }

    thetax *= M_PI/180;
    thetay *= M_PI/180;

    rw::math::Rotation3D rotationZ(cos(thetaz), -sin(thetaz),   .0,
                                   sin(thetaz),  cos(thetaz),   .0,
                                            .0,           .0,   1.);

    rw::math::Rotation3D rotationY(cos(thetay),     .0, -sin(thetay),
                                            .0,     1.,           .0,
                                   sin(thetay),     .0, cos(thetay));

    rw::math::Rotation3D rotationX(1.,          0.,           0.,
                                   0., cos(thetax), -sin(thetax),
                                   0., sin(thetax),  cos(thetax));

    //rw::math::Rotation3D totalRotation = rotationZ*rotationY*rotationX;
    rw::math::Rotation3D totalRotation = rotationX;

    std::cout << "radius: " << radius << " ThetaX: " << thetax << " ThetaY: " << thetay << " ThetaZ: " << thetaz << " vect: " << vect << std::endl;
//    std::cout << totalRotation << std::endl;

    return totalRotation;
}

std::vector<double> xMath::ball_position_to_robotframe(std::tuple<bool, cv::Mat, cv::Point2f, float> data)
{
    if (std::get<0>(data)) {
        cv::Point3d pointRobot(std::get<2>(data).x, std::get<2>(data).y, (std::get<3>(data) - 4.5)); //-3.5 is for the base z offset

     //       cos(-66)  -sin(-66)  0
     //       sin(-66)   cos(-66)  0 men da y akse skal flippes kan man tilføje (-) foran elementerne i denne række.
     //          0         0       1
            rw::math::Rotation3D rotationMatrix( cos(baseRadian), -sin(baseRadian), 0.0,
                                                -sin(baseRadian), -cos(baseRadian), 0.0,
                                                             0.0,              0.0, 1.0 );

            rw::math::Vector3D<double> preposition(pointRobot.x,pointRobot.y,pointRobot.z);
            preposition = rotationMatrix * (preposition/100);


            //rw::math::Vector6D<double> position(preposition(0),preposition(1), preposition(2), 0.720, -3.062, 0.044); // celle 2
            rw::math::Vector6D<double> position(preposition(0),preposition(1), preposition(2), 1.778, 2.577, -0.010); //celle 1-4

                std::cout << "pre-transform: " << position << std::endl;

            //rw::math::Rotation3D rotationTCP = calculateTCPRotation(position);



            //rw::math::Transform3D transform(position, rotationTCP);

            //rw::math::Pose6D pose(transform);

            //std::cout << transform << " | " << pose <<  std::endl;

            std::vector<double> pointAndRotation;

            for (size_t i = 0; i < 6; ++i) {
                pointAndRotation.push_back(position(i));
            }


            //pointRobot = rotationMatrix * pointRobot; //calc new values for position in baseframe

            //pointRobot /= 100; // point starts as cm, but needs to be in M.

            //convert to vector
//            std::vector<double> pointAndRotation;
//            pointAndRotation.push_back(pointRobot.x);
//            pointAndRotation.push_back(pointRobot.y);
//            pointAndRotation.push_back(pointRobot.z);
//            pointAndRotation.push_back(0.720);
//            pointAndRotation.push_back(-3.062);
//            pointAndRotation.push_back(0.044);
//            pointAndRotation.push_back(1.778);
//            pointAndRotation.push_back(2.577);
//            pointAndRotation.push_back(-0.010);



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

void xMath::calcThrow(std::vector<double> &q, double x, double y){
    double r = sqrt(pow(x,2) + pow(y,2));
    double q0 = atan2(y,x) + (deg2rad(22.25) + asin(10.93/r));

    double l = sqrt(pow(r,2) - pow(10.93,2));

    double q1 = -0.00017959125445504*pow(l,2) + 0.0458994035661245*l -4.22833522965417;

    q[0] = q0;
    q[1] = q1;
}
