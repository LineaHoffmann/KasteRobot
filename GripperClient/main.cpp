#include <iostream>
#include "gripperclient.h"

int main()
{
    std::cout << "Group Alfa gripper client protocol initiated" << std::endl;


    GripperClient *client = new GripperClient();
    client->ConnectSocket();


    //GripperClient::ConnectSocket();

    char commandBuf[32];
    std::cout << "Enter command: " << std::endl;
    std::cin.getline(commandBuf, sizeof(commandBuf));
    std::string command(commandBuf);
//    if ((command) != "VERBOSE" OR "BYE" OR "HOME" OR "MOVE" OR "GRIP" OR "RELEASE" OR "PWT"
//            OR "CLT" OR "POS" OR "SPEED" OR "FORCE" OR "GRIPSTATE" OR "STOP" OR "TARE"
//            OR "GRIPSTATS" OR "FDATA" OR "FTYPE" OR "FFLAGS") {
//        std::cout << "Command not valid ";
//        std::cin.getline(commandBuf, size(commandBuf));
//        std::string command = std::string command(commandBuf);

    std::string answer = client->WriteRead(command);
    std::cout << answer << std::endl;


    return 1;
}
