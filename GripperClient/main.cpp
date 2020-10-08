#include <iostream>
#include "gripperclient.h"

int main()
{
    std::cout << "Group Alfa gripper client protocol initiated" << std::endl;

    GripperClient *client = new GripperClient();
    client->connectSocket("192.168.0.1", 1000);
    std::string command;
    std::string answer = client->writeRead(command);




    return 1;
}
