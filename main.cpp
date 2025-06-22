#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <bits/ostream.tcc>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed: " << WSAGetLastError() << '\n';
        return 1;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cout << "Version 2.2 of Winsock not available.\n";
        WSACleanup();
        return 1;
    }
    std::cout << "Winsock initialized successfully.\n";

    const SOCKET listenSocket{socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};

    if (listenSocket == INVALID_SOCKET) {
        std::cout << "socket creation failed with error: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    constexpr int value{1};
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&value), sizeof(value));

    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_port = htons(1234);
    service.sin_addr.s_addr = htonl(INADDR_ANY);

    if (const int bindResult{bind(listenSocket, reinterpret_cast<SOCKADDR *>(&service), sizeof(service))}; bindResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << '\n';
    }

    std::cout << "listening on socket...\n";

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
