#ifndef ROBOTEXCEPTIONS_H
#define ROBOTEXCEPTIONS_H

#include <exception>


class UR_BadIP : public std::system_error{

    const char* what() const throw()
    {
        return "Connection to host could not be resolved";
    }
};

class UR_NotConnected : public std::system_error{

    const char* what() const throw()
    {
        return "Robot connections have not been established!";
    }
};


#endif // ROBOTEXCEPTIONS_H
