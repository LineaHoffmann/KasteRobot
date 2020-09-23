#include "gripperclient.h"

GripperClient::GripperClient()
{
    worker = new std::thread(&GripperClient::entrySocket); // Uendeligt lopp, der evt. kan disconnect men ellers køre. Sæt funktionalitet i tråd med mutex.
}

//void GripperClient::CreateSocket() {
//    int sock_ = socket(AF_INET, SOCK_STREAM, 0);
// }


void GripperClient::ConnectSocket() {
    int sock_ = socket(AF_INET, SOCK_STREAM, 0);
    int port = 1000;
    std::string ipAddress = "192.168.100.10";

    sockaddr_in hint_;
    hint_.sin_family = AF_INET;
    hint_.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint_.sin_addr);

    connect(sock_, (sockaddr*)&hint_, sizeof(hint_));
    return;
}

std::string GripperClient::WriteRead(std::string command_) {
    char buf[32];
    int bytesRecieved = -1; //Resetting response length
    int sendRes = send(sock_, command_.c_str(), command_.size() + 1, 0); // Sending command

    memset(buf, 0, 32);
    bytesRecieved = recv(sock_, buf, 32, 0); //Reading response
        if (bytesRecieved == -1) {              //Waiting and retrying if no response
            bytesRecieved = recv(sock_ ,buf, 32, 0);
            usleep(250000);  //Microseconds; 0,25 second
        }
     std::string answer_(buf);
     return answer_;
}
