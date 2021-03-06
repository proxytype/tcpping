// pinger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <signal.h>
#include <stdio.h>
#include <chrono>

#define DEFAULT_BUFLEN 512
#pragma comment (lib, "Ws2_32.lib")

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
typedef std::chrono::high_resolution_clock Clock;

const uint64_t numerator = (1LL << 32) / 1000000;
const char * ARGS_LOOP = "loop";
const char * ARGS_SINGLE = "single";

HANDLE hConsole = NULL;
WORD attributes = 0;

void printError(const char * error, int exitcode) {

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY);

    printf("%s\n", error);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);

    exit(exitcode);
}

void showHeader() {
    SetConsoleTextAttribute(hConsole,
        FOREGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    printf(" _____         ______ _             \n");
    printf("|_   _|        | ___ (_)            \n");
    printf("  | | ___ _ __ | |_/ /_ _ __   __ _ \n");
    printf("  | |/ __| '_ \\|  __/| | '_ \\ / _` |\n");
    printf("  | | (__| |_) | |   | | | | | (_| |\n");
    printf("  |_|\\___| .__/\\_|   |_|_| |_|\\__, |\n");
    printf("         | |                   __/ |\n");
    printf("         |_|                  |___/ \n");
    printf("---------------------------------------------------\n");
    printf("[tcpping] by: RudeNetworks.com | version: 0.1 beta\n");
}

void showOptions() {

    printf(" - tcpping.exe <ip> <port> <single/loop>\n");
    printf("   options:\n");
    printf("    xxx.xxx.xxx.xxx               :: ip address   \n");
    printf("    1-65535                       :: port         \n");
    printf("    loop                          :: loop, single \n");
}

int toLower(char* txt) {
    int len = strlen(txt);
    for (int i = 0; i < len; i++)
    {
        txt[i] = tolower(txt[i]);
    }

    return len;
}

bool makeTcpConnection(char* ip, char* port, char *mode) {

    auto t1 = Clock::now();
    
    WSADATA wsaData;
    SOCKET ConnectSocket = strtol(port, NULL, 10);

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printError("WSAStartup failed", -1);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, port, &hints, &result);

    if (iResult != 0) {
        WSACleanup();
        printError("getaddrinfo failed", -1);
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            WSACleanup();
            printError("socket failed", -1);
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    auto t2 = Clock::now();

    nanoseconds ns = duration_cast<nanoseconds>(t2 - t1);

    int millionths = (ns.count() * numerator) >> 32;
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("%s::%s -> closed! - %dms\n", ip, port, millionths);
        WSACleanup();
    }
    else {
        printf("%s::%s -> connected! - %dms\n", ip, port, millionths);
        closesocket(ConnectSocket);
        WSACleanup();
    }

    if (strcmp(mode, ARGS_LOOP) == 0) {
        Sleep(1000);
        makeTcpConnection(ip, port, mode);
    }

    return TRUE;
}

void  INThandler(int sig)
{
    char  c;

    signal(sig, SIG_IGN);
    printf("exit...");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, INThandler);
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO Info;
    GetConsoleScreenBufferInfo(hConsole, &Info);
    attributes = Info.wAttributes;

    showHeader();

    if (argc < 3 || argc > 4) {
        showOptions();
        return -1;
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    char*ip = argv[1];
    char* port = argv[2];
    char* mode = (char*)"single";

    if (argc == 4) {
        mode = argv[3];
    }

    toLower(mode);
    makeTcpConnection(ip, port, mode);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
    return 0;
}



