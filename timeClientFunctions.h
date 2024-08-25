#ifndef __TIME_CLIENT_FUNCTIONS_H
#define __TIME_CLIENT_FUNCTIONS_H

#include <winsock2.h>
#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <string>


using namespace std;
#define NUMBER_0F_MULTIPLE_REQUESTS 100

enum eOption {
    GET_TIME = 1, GET_TIME_WITHOUT_DATE, GET_TIME_SINCE_EPOCH,
    GET_CLIENT_TO_SERVER_DELAY_ESTIMATION, MEASURE_RTT,
    GET_TIME_WITHOUT_DATE_OR_SECONDS, GET_YEAR, GET_MONTH_AND_DAY,
    GET_SECONDS_SINCE_BEGINING_OF_MONTH, GET_WEEK_OF_YEAR, GET_DAYLIGHT_SAVINGS,
    GET_TIME_WITHOUT_DATE_IN_CITY, MEASURE_TIME_LAP, EXIT = -1
};

enum eSupportedCities {
    DOHA = 1, PRAGUE, NEW_YORK, BERLIN, UTC
};

void printMenu();
void printCityMenu();
void sendRequest(int userChoice, SOCKET& connSocket, const sockaddr_in& server);
string receiveResponse(SOCKET& connSocket);
void initializeSocket(SOCKET& connSocket);
void cleanup(SOCKET& connSocket);
string handleUserChoice(int userChoice, SOCKET connSocket, sockaddr_in server, bool& exit);
int requestsCityFromClient();
string handleCityRequest(SOCKET& connSocket, const sockaddr_in& server);
string getClientToServerDelayEstimation(int numberOfRequests, SOCKET connSocket, sockaddr_in server);
double reciveResponsesAvgDelay(int numOfResponses, SOCKET connSocket, sockaddr_in& server);
string measureRTT(int numOfResponses, SOCKET connSocket, sockaddr_in& server);


#endif // __TIME_CLIENT_FUNCTIONS_H