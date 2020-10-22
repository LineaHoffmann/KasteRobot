#ifndef XEXCEPTIONS
#define XEXCEPTIONS

#include <system_error>

namespace x_err {
struct what {
    inline static std::string ROBOT_BAD_IP = "Connection to host could not be resolved!";
    inline static std::string ROBOT_NOT_CONNECTED = "Robot connections have not been established!";
};
class error : public std::runtime_error {
public:
    error(const std::string& msg) : std::runtime_error(msg.c_str()) {}
};
}

#endif // XEXCEPTIONS
