#include <iostream>
#include "gripperclient.h"

int main()
{
    std::cout << "Group Alfa gripper client protocol initiated" << std::endl;


    GripperClient *client = new GripperClient();
    client->ConnectSocket();

    std::string mCommand;
    std::string mAnswer = client->WriteRead(mCommand);
    std::cout << mAnswer << std::endl;


    return 1;
}
