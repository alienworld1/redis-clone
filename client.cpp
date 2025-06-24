#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

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

    const SOCKET clientSocket{socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in clientService{};
    clientService.sin_family = AF_INET;
    clientService.sin_port = ntohs(1234);
    clientService.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

    if (const int result{connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService))}; result == SOCKET_ERROR) {
        std::cout << "connection failed: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    constexpr char message[]{"hello"};
    send(clientSocket, message, std::size(message), 0);

    char readBuffer[64]{};
    if (const int bytesRead{recv(clientSocket, readBuffer, sizeof(readBuffer) - 1, 0)}; bytesRead < 0) {
        std::cout << "error reading from socket: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "server says: " << readBuffer << '\n';

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}