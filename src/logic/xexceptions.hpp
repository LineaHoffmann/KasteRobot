#ifndef XEXCEPTIONS
#define XEXCEPTIONS

#include <system_error>

/**
 * This header contains all errors thrown by logic classes
 * To create a new error, define a new inline static const std::string in x_err::what
 *
 * This header is meant to be the full implementation, and requires >=C++17 because of
 * the definition of static objects in header (inline not previously supported here)
 *
 * Throw errors like (example):
 *      throw x_err::error(x_err::what::ROBOT_BAD_IP);
 *
 * Catch errors like (example):
 *      try {...} catch (x_err::error &e) {...}
 *
 * If an error is caught and you want to know the type, test it with (example):
 *      if (e.what() == x_err::what::ROBOT_BAD_IP) {...}
 */

// TODO: Start implementing these whenever you need some errors
// The below errors are merely examples, and can be changed as required

namespace x_err {
struct what {
    inline static const std::string NO_IMPLEMENTATION = "This is not implemented in the xController class yet .. ";

    inline static const std::string ROBOT_BAD_IP = "Connection to host could not be resolved! ";
    inline static const std::string ROBOT_NOT_CONNECTED = "Robot connections have not been established! ";

    inline static const std::string CAMERA_GRAB_ERROR = "Error in camera image grabbing loop! ";
    inline static const std::string CAMERA_NOT_CONNECTED = "Camera is not connected! ";
    inline static const std::string CAMERA_NOT_STARTED = "Camera has not been started! ";
    inline static const std::string CAMERA_WRONG_PATH = "Camera path variable is faulty! ";

    inline static const std::string GRIPPER_NOT_STARTED = "Gripper has not been started! ";
    inline static const std::string GRIPPER_NO_ACK = "Gripper did not respond with ACK! ";

    inline static const std::string ROBWORK_INVALID_DATA = "RobWork was given invalid data! ";
};
class error : public std::runtime_error {
public:
    error(const std::string& msg) : std::runtime_error(msg.c_str()) {}
};
}

#endif // XEXCEPTIONS
