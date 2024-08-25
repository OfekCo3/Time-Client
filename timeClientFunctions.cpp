#include "timeClientFunctions.h"

void printMenu()
{
    cout << "\nPlease enter a number to choose an option (enter " << EXIT << " to exit):\n"
        << "------------------------------------------\n"
        << "(" << GET_TIME << ") Get the year, month, day, hour, minute, second\n"
        << "(" << GET_TIME_WITHOUT_DATE << ") Get the hour, minute, second\n"
        << "(" << GET_TIME_SINCE_EPOCH << ") Get the seconds passed since 01.01.1970\n"
        << "(" << GET_CLIENT_TO_SERVER_DELAY_ESTIMATION << ") Print the delay between the client and the server\n"
        << "(" << MEASURE_RTT << ") Get the RTT\n"
        << "(" << GET_TIME_WITHOUT_DATE_OR_SECONDS << ") Get hour and minute\n"
        << "(" << GET_YEAR << ") Get the year\n"
        << "(" << GET_MONTH_AND_DAY << ") Get the month and day\n"
        << "(" << GET_SECONDS_SINCE_BEGINING_OF_MONTH << ") Get the seconds since the beginning of the month\n"
        << "(" << GET_WEEK_OF_YEAR << ") Get the weeks since the beginning of the year\n"
        << "(" << GET_DAYLIGHT_SAVINGS << ") If it's Daylight Saving Time get 1 else 0\n"
        << "(" << GET_TIME_WITHOUT_DATE_IN_CITY << ") Get the hour, minute, and second in a specific city\n"
        << "(" << MEASURE_TIME_LAP << ") Measure time lap\n";
}

void printCityMenu()
{
    cout << "\nPlease enter a number to choose the city:\n"
        << "------------------------------------------\n"
        << "(" << DOHA << ") Doha\n"
        << "(" << PRAGUE << ") Prague\n"
        << "(" << NEW_YORK << ") New York\n"
        << "(" << BERLIN << ") Berlin\n"
        << "(" << UTC << ") UTC\n";
}

void sendRequest(int userChoice, SOCKET& connSocket, const sockaddr_in& server)
{
    char sendBuff[2];
    sendBuff[0] = static_cast<char>(userChoice);
    sendBuff[1] = '\0';

    int bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
    if (SOCKET_ERROR == bytesSent)
    {
        cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
        cleanup(connSocket);
        exit(1);
    }
    cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << userChoice << "\" message.\n";
}

string receiveResponse(SOCKET& connSocket)
{
    char recvBuff[255];
    int bytesRecv = recv(connSocket, recvBuff, 255, 0);
    if (SOCKET_ERROR == bytesRecv)
    {
        cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
        cleanup(connSocket);
        exit(1);
    }
    recvBuff[bytesRecv] = '\0'; // Null-terminate the string
    std::string strResponse = recvBuff;
    return strResponse;
}

void initializeSocket(SOCKET& connSocket)
{
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "Time Client: Error at WSAStartup()\n";
        exit(1);
    }

    connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == connSocket)
    {
        cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        exit(1);
    }
}

void cleanup(SOCKET& connSocket)
{
    if (connSocket != INVALID_SOCKET)
    {
        closesocket(connSocket);
        WSACleanup();
    }
}

string handleUserChoice(int userChoice, SOCKET connSocket, sockaddr_in server, bool& exit)
{
    string response;
    eOption eUserChoice = (eOption)userChoice;

    if ((userChoice < GET_TIME && userChoice != EXIT) || userChoice > MEASURE_TIME_LAP)
    {
        response = "Please Enter a valid number.\n";
    }
    else
    {
        switch (userChoice)
        {
        case GET_CLIENT_TO_SERVER_DELAY_ESTIMATION:
            response = getClientToServerDelayEstimation(NUMBER_0F_MULTIPLE_REQUESTS, connSocket, server);
            break;
        case MEASURE_RTT:
            response = measureRTT(NUMBER_0F_MULTIPLE_REQUESTS, connSocket, server);
            break;
        case GET_TIME_WITHOUT_DATE_IN_CITY:
            response = handleCityRequest(connSocket, server);
            break;
        case EXIT:
            response = "Exiting program.";
            exit = true; 
            break;
        default:
            sendRequest(userChoice, connSocket, server);
            response = receiveResponse(connSocket);
            break;
        }
    }

    return response;
}
int requestsCityFromClient()
{
    bool validChoice = false;
    int userChoice;

    do
    {
        printCityMenu();
        cin >> userChoice;
        if (userChoice >= DOHA && userChoice <= UTC)
        {
            validChoice = true;
        }
        else
        {
            cout << "Please enter a valid number.\n";
        }
    } while (!validChoice);

    return userChoice;
}

string handleCityRequest(SOCKET& connSocket, const sockaddr_in& server)
{
    int userChoice;
    string serverResponce;

    sendRequest(GET_TIME_WITHOUT_DATE_IN_CITY, connSocket, server);
    cout << receiveResponse(connSocket) << endl;
    userChoice = requestsCityFromClient();
    sendRequest(userChoice, connSocket, server);
    serverResponce = receiveResponse(connSocket);
    return serverResponce;
}

string getClientToServerDelayEstimation(int numberOfRequests, SOCKET connSocket, sockaddr_in server)
{
    for (int i = 0; i < numberOfRequests; i++)
    {
        sendRequest(GET_CLIENT_TO_SERVER_DELAY_ESTIMATION, connSocket, server);
    }

    return to_string(reciveResponsesAvgDelay(numberOfRequests, connSocket, server));
}

double reciveResponsesAvgDelay(int numOfResponses, SOCKET connSocket, sockaddr_in& server)
{
    string currResponse;
    string lastResponse;
    double timeSum = 0;

    currResponse.resize(255);
    lastResponse.resize(255);

    lastResponse = receiveResponse(connSocket);

    for (int i = 0; i < numOfResponses - 1; i++)
    {
        currResponse = receiveResponse(connSocket);
        timeSum += stod(currResponse) - stod(lastResponse);
        lastResponse = currResponse;
    }

    return (timeSum / numOfResponses);
}

string measureRTT(int numOfResponses, SOCKET connSocket, sockaddr_in& server)
{
    double timeSum = 0;
    double sendTime = 0;
    double reciveTime = 0;

    for (int i = 0; i < numOfResponses; i++)
    {
        sendRequest(MEASURE_RTT, connSocket, server);
        sendTime = GetTickCount();
        receiveResponse(connSocket);
        reciveTime = GetTickCount();
        timeSum += reciveTime - sendTime;
    }

    return to_string(timeSum / numOfResponses);
}