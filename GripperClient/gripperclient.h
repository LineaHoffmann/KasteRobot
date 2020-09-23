#ifndef GRIPPERCLIENT_H
#define GRIPPERCLIENT_H
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <thread>


class GripperClient
{
public:
    GripperClient();
    void CreateSocket();
    void ConnectSocket();
    std::string WriteRead(std::string command_);

private:
    int sock_, connectS_;
    std::string ipAddress_, command_, userInput_, answer_;
    sockaddr_in hint_;

    std::thread *worker;

};


#endif // GRIPPERCLIENT_H
