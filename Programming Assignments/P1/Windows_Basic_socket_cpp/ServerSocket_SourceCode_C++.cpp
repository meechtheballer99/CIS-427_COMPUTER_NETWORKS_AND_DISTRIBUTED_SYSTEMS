#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA WSAData;

    SOCKET serverSock, clientSock;

    SOCKADDR_IN serverAddr, clientAddr;

    WSAStartup(MAKEWORD(2,0), &WSAData);
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    bind(serverSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 0);

    cout << "Listening for incoming connections..." << endl;

    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);
    if((clientSock = accept(serverSock, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        cout << "Client connected!" << endl;
        recv(clientSock, buffer, sizeof(buffer), 0);
        cout << "Client says: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer));

        closesocket(clientSock);
        cout << "Client disconnected." << endl;
    }
}
