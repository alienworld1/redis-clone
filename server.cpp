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
#include <bits/ostream.tcc>
#include "utils/socketio.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr int maxBufferSize{4096};

static void readAndWrite(const SOCKET socket) {
    char readBuffer[64]{};

    if (const int bytesRead{recv(socket, readBuffer, sizeof(readBuffer) - 1, 0)}; bytesRead < 0) {
        std::cerr << "error reading from socket: " << WSAGetLastError() << '\n';
        return;
    }

    std::cout << "client says: " << readBuffer << '\n';

    constexpr char writeBuffer[]{"world"};
    send(socket, writeBuffer, sizeof(writeBuffer) - 1, 0);
}

static int oneRequest(const SOCKET socket) {
    std::cout << "reading from socket\n";

    char readBuffer[4 + maxBufferSize]{};
    errno = 0;

    if (const int err{SocketIO::readFull(socket, readBuffer, 4)}) {
        std::cerr << (errno == 0 ? "EOF": "read error") << " on socket: " << WSAGetLastError() << '\n';
        return err;
    }

    uint32_t size{0};
    std::memcpy(&size, readBuffer, 4);

    if (size > maxBufferSize) {
        std::cerr << "message size too long: " << size << '\n';
        return -1;
    }

    if (const int err{SocketIO::readFull(socket, readBuffer + 4, size)}; err) {
        std::cerr << "error reading from socket: " << WSAGetLastError() << '\n';
        return err;
    }

    std::cout << "client says: " << readBuffer + 4 << '\n';

    constexpr char reply[]{"world"};
    char writeBuffer[4 + sizeof(reply)]{};

    size = std::size(reply) - 1;
    std::memcpy(writeBuffer, &size, 4);
    std::memcpy(writeBuffer + 4, &reply, size);
    return SocketIO::writeAll(socket, writeBuffer, static_cast<int>(size + 4));
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

    const SOCKET listenSocket{socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};

    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket creation failed with error: " << WSAGetLastError() << '\n';
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
        std::cerr << "bind failed with error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << '\n';
    }


    while (true) {
        std::cout << "listening on socket...\n";

        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);

        const SOCKET clientSocket{accept(listenSocket, reinterpret_cast<SOCKADDR *>(&clientAddress), &clientAddressLength)};

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << '\n';
            continue;
        }

        std::cout << "Accepted connection from " << inet_ntoa(clientAddress.sin_addr) << ':' << ntohs(clientAddress.sin_port) << '\n';

        while (true) {
            if (const int err{oneRequest(clientSocket)}) {
                std::cerr << "error processing request: " << err << '\n';
                break;
            }
        }
        closesocket(clientSocket);
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
