#include <cassert>
#include "socketio.h"

int32_t SocketIO::readFull(const SOCKET socket, char* buffer, int size) {
    while (size > 0) {
        const int bytesRead{recv(socket, buffer, size, 0)};
        if (bytesRead <= 0) {
            // error or unexpected EOF
            return -1;
        }
        assert(bytesRead <= size);
        size -= bytesRead;
        buffer += bytesRead;
    }
    return 0;
}

int32_t SocketIO::writeAll(const SOCKET socket, const char *buffer, int size) {
    while (size > 0) {
        const int bytesWritten{send(socket, buffer, size, 0)};
        if (bytesWritten <= 0) {
            return -1;
        }
        assert(bytesWritten <= size);
        size -= bytesWritten;
        buffer += bytesWritten;
    }
    return 0;
}

