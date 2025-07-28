//Author: Demetrius Johnson
//For: CIS-427 With Dr. Zheng Song at UM-Dearborn
//Date created: 10-21-22
//Date modified: 10-22-22
//purpose: Create the client for the client-server socket programming application 
    //to get and update BANNER message of the day service ran on the remote server.

#include <iostream>
#include <Winsock2.h> //need this for socket programing in Windows
#include <Ws2tcpip.h> //need this for updated functions that replace depracated functions (such as inet_addr() function)
#define APP_SOCKET_PORT 5555
using namespace std;

//this program's main function takes 1 commmandline parameter: server ip address as per the requirement for this assignment.

int main(int argc, char** argv ) //remember: element 0 in the vector is the file name; first commandline argument begins at element 1
{
    WSADATA WSAData;
    SOCKET serverSock; //note: SOCKET is a descriptor to identify SOCKADDR_IN structures; we will use serverSock/addr to connect to the server via IP address and TCP port#
    SOCKADDR_IN addr;  // The sockaddr_in structure specifies the address family, IP address, and port of the server to be connected to.

    WSAStartup(MAKEWORD(2,0), &WSAData);            //initiates use of the Winsock DLL by a process; we are using Winsock v 2.0 --> 2,0
    serverSock = socket(AF_INET, SOCK_STREAM, 0);   //AF_INET specifies IPv4, SOCK_STREAM means use TCP, 0 means do not use a specific protocol (i.e. 1 = ICMP)
    char ip_address[16] = "127.0.0.1";              //default ip will be loopback address of local machine

    //only update IP with command line parameter if there is one present and if no more than 1 parameter has been passed in.
    if (argc == 2)
        strcpy_s(ip_address, argv[1]);

    //inet_pton() function converts ASCII string to binary IP address to be stored in addr object --> assign the IP to the addr socket.
    //AF_INET specifies IPV4 
    //addr.sin_port = htons(port #) assigns port# to the addr socket.

    //inet_pton() function will convert the IPv4 string into binary and store the address in the the struct in the proper field (replaces the depracated inet_addr() function)
    inet_pton(AF_INET, ip_address, &addr.sin_addr.s_addr);

    addr.sin_family = AF_INET;   
    addr.sin_port = htons(APP_SOCKET_PORT);

    connect(serverSock, (SOCKADDR*)&addr, sizeof(addr));   //try to connect to server using server IP and Port number we would like to connect on

    //WSAGetLastError allows you to get the most recent error code (if there were any) stored in a winsock2.h variable
    if (WSAGetLastError() == 10060) //error code 10060 means connection timed out (failed)
    {
        cout << "...connection timed out...\n...exiting program...\n";
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }
    if (WSAGetLastError() == 10061) //error code 10061 means connection refused by host/target machine (server refused connection)
    {
        cout << "...connection refused by target machine...\n...exiting program...\n";
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!" << endl;

    //test sending messages to server on serverSock:
    char buffer[1024]={'h', 'e', 'l', 'l', 'o', '.'};
    send(serverSock, buffer, sizeof(buffer), 0);
    cout << "Message sent!" << endl;


    //test receiving messages from server on serverSock:
    memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
    recv(serverSock, buffer, sizeof(buffer), 0);
    cout << "Server says: " << buffer << endl;

    //////////////////////////////////////////////////////////////////////
    //READY TO BEGIN REQUESTING YAMOTD Application services from server:
    /////////////////////////////////////////////////////////////////////
    char REQUEST[20];

    //NOTE: if strings are EQUAL, strcmp (string compare) returns 0
   do {
       //get request input from USER:
       cout << "Enter a request to send to YAMOTD banner server (MSGGET, MSGSTORE, or QUIT): ";
       cin >> REQUEST;          //get input from user
       strcat_s(REQUEST, "\n");  //concatenate newline character to input as required for this client-server application

        //send request to server:
        memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
        strcpy_s(buffer, REQUEST); //copy user request message into buffer
        send(serverSock, buffer, sizeof(buffer), 0); //send request to server
        cout << "Message sent!" << endl;

        //Receive request response from server:
        memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
        recv(serverSock, buffer, sizeof(buffer), 0);
        cout << "Server says: " << buffer << endl;

        //case: requesting to update the banner
        if (strcmp(REQUEST, "MSGSTORE\n") == 0) {
        
            //send new banner to server:
            memset(buffer, 0, sizeof(buffer));  //reset buffer (zero it --> set all values in the array to 0)
            cin.ignore(sizeof(buffer), '\n');   //need to flush cin buffer before the next operation
            cin.getline(buffer, 200, '\n');     //banner is limited by 200 characters
            strcat_s(buffer, "\n");             //concatenate newline character to input as required for this client-server application
            send(serverSock, buffer, sizeof(buffer), 0); //send banner to server
            cout << "Message sent!" << endl;
        
            //Receive update response from server:
            memset(buffer, 0, sizeof(buffer)); //reset buffer (zero it --> set all values in the array to 0)
            recv(serverSock, buffer, sizeof(buffer), 0);
            cout << "Server says: " << buffer << endl;
        
        }

   } while (strcmp(REQUEST, "QUIT\n") != 0);


    closesocket(serverSock);
    WSACleanup();
    cout << "Socket closed." << endl << endl;

    system("pause");
    return 0;
}
