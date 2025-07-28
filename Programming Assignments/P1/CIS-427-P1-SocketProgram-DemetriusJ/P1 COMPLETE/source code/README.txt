1) make sure when you compile that the ws2_32.lib library is added in order to use the winsock2.h functions.
 It contains the definitions of the declarations contained in the header files:
#include <Winsock2.h> //need this for socket programing in Windows
#include <Ws2tcpip.h> //need this for updated functions that replace depracated functions (such as inet_addr() function)


2) you need the above library because I wrote my application in C++ as a Windows Client-Server application, where both the client and server are Microsoft Windows machines.