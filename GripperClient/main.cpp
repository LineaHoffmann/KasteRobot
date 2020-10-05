#include <iostream>
#include "gripperclient.h"

int main()
{
    std::cout << "Group Alfa gripper client protocol initiated" << std::endl;

    GripperClient *client = new GripperClient();
    client->ConnectSocket("192.168.0.1", 1000);
    std::string command;
    std::string answer = client->WriteRead(command);




    return 1;
}
