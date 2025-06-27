#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <cstring>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "utils/socketio.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr int maxBufferSize{4096};

static int query(SOCKET socket, const char* message) {
    uint32_t messageSize{static_cast<uint32_t>(strlen(message))};
    if (messageSize > maxBufferSize) {
        return -1;
    }

    char writeBuffer[4 + maxBufferSize];
    std::memcpy(writeBuffer, &messageSize, 4);
    std::memcpy(writeBuffer + 4, message, messageSize);

    if (const int err{SocketIO::writeAll(socket, writeBuffer, static_cast<int>(messageSize + 4))}; err) {
        return err;
    }

    char readBuffer[4 + maxBufferSize + 1];
    errno = 0;

    if (const int err{SocketIO::readFull(socket, readBuffer, 4)}; err) {
        std::cerr << (errno == 0 ? "EOF" : "read error") << " on socket: " << WSAGetLastError() << '\n';
        return err;
    }

    std::memcpy(&messageSize, readBuffer, 4);
    if (messageSize > maxBufferSize) {
        std::cerr << "message size too long: " << messageSize << '\n';
        return -1;
    }

    if (const int err{SocketIO::readFull(socket, readBuffer + 4, static_cast<int>(messageSize))}; err) {
        std::cerr << "error reading from socket: " << WSAGetLastError() << '\n';
        return err;
    }

    std::cout << "server says: " << std::string{readBuffer + 4, messageSize} << '\n';
    return 0;

}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << '\n';
        return 1;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cerr << "Version 2.2 of Winsock not available.\n";
        WSACleanup();
        return 1;
    }
    std::cout << "Winsock initialized successfully.\n";

    const SOCKET clientSocket{socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in clientService{};
    clientService.sin_family = AF_INET;
    clientService.sin_port = ntohs(1234);
    clientService.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

    if (const int result{connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService))}; result == SOCKET_ERROR) {
        std::cerr << "connection failed: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // constexpr char message[]{"hello"};
    // send(clientSocket, message, std::size(message), 0);
    //
    // char readBuffer[64]{};
    // if (const int bytesRead{recv(clientSocket, readBuffer, sizeof(readBuffer) - 1, 0)}; bytesRead < 0) {
    //     std::cout << "error reading from socket: " << WSAGetLastError() << '\n';
    //     closesocket(clientSocket);
    //     WSACleanup();
    //     return 1;
    // }
    //
    // std::cout << "server says: " << readBuffer << '\n';

    int32_t error{query(clientSocket, "hello1")};
    if (error) {
        std::cerr << "query failed with error: " << error << '\n';
        goto LDONE;
    }
    error = query(clientSocket, "hello2");
    if (error) {
        std::cerr << "query failed with error: " << error << '\n';
        goto LDONE;
    }

LDONE:
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}