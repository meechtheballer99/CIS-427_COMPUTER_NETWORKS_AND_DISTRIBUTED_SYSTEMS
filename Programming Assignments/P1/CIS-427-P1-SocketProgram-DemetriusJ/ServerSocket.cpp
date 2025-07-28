//Author: Demetrius Johnson
//For: CIS-427 With Dr. Zheng Song at UM-Dearborn
//Date created: 10-21-22
//Date modified: 10-22-22
//purpose: Create the server for the client-server socket programming application 
    //to send and update BANNER message of the day service ran on the server.

#include <iostream>
#include <Winsock2.h> //need this for socket programing in Windows
#include <Ws2tcpip.h> //need this for updated functions that replace depracated functions (such as inet_addr() function)
#define APP_SOCKET_PORT 5555
using namespace std;

int main()
{
    WSADATA WSAData;    //WinSock data structure

    SOCKET serverSock, clientSock; //note: SOCKET is a descriptor to identify SOCKADDR_IN structures: use serverSock to listen to incoming connection requests; use clientSock for accepting incoming requests;
                                   //We do this so that the server can listen and accept requests at the same time.

    SOCKADDR_IN serverAddr, clientAddr;  // The sockaddr_in structure specifies the address family, IP address, and port of the server to be connected to.

    int check_WSAStartup = WSAStartup(MAKEWORD(2, 0), &WSAData); //initiates use of the Winsock DLL by a process; we are using Winsock v 2.0 --> 2,0
    if (check_WSAStartup != NO_ERROR) {
    
        wprintf(L"WSAStartup function failed with error: %d\n", check_WSAStartup);
        return 1;
    }

    //initialize socket and get descriptor
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    //socket initialization error checking:
    if (serverSock == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


    //inet_pton() function converts ASCII string to binary IP address to be stored in addr object --> assign the IP to the serverAddr socket.
    //AF_INET specifies IPV4 
    //addr.sin_port = htons(port #) assigns port# to the addr socket.

    serverAddr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY means listen to all ip addresses of all interfaces on the local machine
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(APP_SOCKET_PORT);

    //serverSock descriptor to serverAddr structure
    bind(serverSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    //bind error checking:
    if (serverSock == SOCKET_ERROR) {
        wprintf(L"bind function failed with error %d\n", WSAGetLastError());
        serverSock = closesocket(serverSock);
        if (serverSock == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //listen on the serverSock and check for errors when we try to listen
    if (listen(serverSock, 0) == SOCKET_ERROR)
        wprintf(L"listen function failed with error: %d\n", WSAGetLastError());

    //WSAGetLastError allows you to get the most recent error code (if there were any) stored in a winsock2.h variable
    if (WSAGetLastError() == 10060) //error code 10060 means connection timed out (failed)
    {
        cout << "...connection timed out...\n...exiting program...\n";
        return 1;
    }

    cout << "Listening for incoming connections..." << endl;

    //create a buffer for sending and receiving data
    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr); //get size of clientAddr struct size

    //now accept a client that arrives at the serverSock that we are listening on; set clentSock descriptor
    //by setting clientAddr structure equal to the connection/structure (SOCKADDR_IN) that serverSock is listening to.
    if((clientSock = accept(serverSock, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        cout << "Client connected!" << endl;

        //receive initial message from client:
        memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
        recv(clientSock, buffer, sizeof(buffer), 0);
        cout << "Client says: " << buffer << endl;


        //send initial message to client:
        cout << "--SENDING a message to Client--" << endl;
        memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
        strcpy_s(buffer, "\n\tWelcome to the Yet Another Message of the Day (YAMOTD) Banner server.\n\t~We are connected over TCP/IP.~\n");
        send(clientSock, buffer, sizeof(buffer), 0);
        cout << "--message SENT to Client--" << endl;
    }
    else {//error checking output 

        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
        cout << "\n...invalid socket...closing connection...\n";
        closesocket(serverSock);
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //CONNECTION IS NOW ESTABLISHED AND TESTED, WE CAN NOW SEND AND RECEIVE ON ClientSocket - BEGIN "YET ANOTHER MESSEAGE OF THE DAY BANNER" (YAMOTD) PROGRAM:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char YAMOTD[200] = "An apple a day keeps the doctor away.\n"; //default banner message
    char MSGGET[20] = "MSGGET\n";
    char MSGSTORE[20] = "MSGSTORE\n";
    char QUIT[20] = "QUIT\n";
    char MSG_200_OK[20] = "200 OK\n\t";
    //NOTE: if strings are EQUAL, strcmp (string compare) returns 0

    //run this loop until client wishes to close the connection
    while (true) {

        //receive message from client:
        memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
        recv(clientSock, buffer, sizeof(buffer), 0);
        cout << "Client says: " << buffer << endl;

     //MSGGET REQUEST:
        //if buffer == MSGGET
        if (strcmp(buffer, MSGGET) == 0) {

            //send 200 OK AND YAMOTD message to client:
            cout << "--SENDING YAMOTD to Client--" << endl;
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            //next 2 lines --> buffer =  MSG_200_OK + YAMOTD:
            strcpy_s(buffer, MSG_200_OK);
            strcat_s(buffer, YAMOTD);
            //send message to client:
            send(clientSock, buffer, sizeof(buffer), 0);
            cout << "--YAMOTD SENT to Client--" << endl;


        }
     //MSGSTORE REQUEST:
        //if buffer == MSFSTORE
        else if (strcmp(buffer, MSGSTORE) == 0) {


            //send "200 OK" message to client:
            cout << "--SENDING '200 OK' to Client--" << endl;
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            strcpy_s(buffer, "200 OK (input banner message terminated with newline, 200 char or less):\n");
            send(clientSock, buffer, sizeof(buffer), 0);
            cout << "--'200 OK' SENT to Client--" << endl;

            //receive message from client:
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            recv(clientSock, buffer, sizeof(buffer), 0);
            cout << "Client YAMOTD Banner reads: " << buffer << endl;

            //store message:
            cout << "~storing message~" << endl;
            memset(YAMOTD, 0, sizeof(YAMOTD)); //reset buffer (zero it --> set all values in the array to 0)
            strcpy_s(YAMOTD, buffer);

            //send confirmation to client:
            cout << "--SENDING banner update confirmation to Client--" << endl;
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            strcpy_s(buffer, "200 OK ~banner updated by server succesfully~\n");
            send(clientSock, buffer, sizeof(buffer), 0);
            cout << "--banner update confirmation SENT to Client--" << endl;


        }
     //QUIT REQUEST:
        //if buffer == QUIT
        else if (strcmp(buffer, QUIT) == 0) {


            //send "200 OK" quit message to client:
            cout << "--SENDING '200 OK' quit confirmation to Client--" << endl;
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            strcpy_s(buffer, "200 OK...closing socket connection...\n");
            send(clientSock, buffer, sizeof(buffer), 0);
            cout << "--'200 OK' quit confirmation SENT to Client--" << endl;

            //close all sockets and exit server application
            closesocket(serverSock);
            closesocket(clientSock);
            WSACleanup();
            cout << "Client disconnected. Exiting server program..." << endl;
            system("pause");
            return 0;
        }
     //INVALID REQUEST:
        else
        {
            //send error message to client:
            cout << "--SENDING error message to Client--" << endl;
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            strcpy_s(buffer, "~error: invalid request. Please try again (MSGGET or MSGSTORE or QUIT)\n");
            send(clientSock, buffer, sizeof(buffer), 0);
            cout << "--error message SENT to Client--" << endl;

        }
    }
}
