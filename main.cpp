#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "timeClientFunctions.h"

#pragma comment(lib, "Ws2_32.lib")

#define TIME_PORT 27015
using namespace std;


int main()
{
    SOCKET connSocket = INVALID_SOCKET;
    initializeSocket(connSocket);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(TIME_PORT);

    int userChoice = 0;
    bool cityChoice = false;
    bool exit = false;
    string response;

    while (!exit)
    {
        printMenu();
        cin >> userChoice;
        response = handleUserChoice(userChoice, connSocket, server, exit); // Pass exit by reference

        if (exit)
        {
            cout << "Bye bye!";
            break; // Exit the loop if the exit flag is set
        }

        cout << "Time Server: " << response << ".\n";
        cout << "Please press enter to continue";
        cin.ignore(); // To handle the enter key
        cin.get();    // Wait for user input
        system("CLS"); // Clear screen
    }

    cleanup(connSocket);
    return 0;
}
