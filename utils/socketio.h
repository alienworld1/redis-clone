//
// Created by astro on 27/6/25.
//

#ifndef SOCKETIO_H
#define SOCKETIO_H

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <winsock2.h>

namespace SocketIO {
    int32_t readFull(SOCKET socket, char* buffer, int size);
    int32_t writeAll(SOCKET socket, const char* buffer, int size);
}

#endif //SOCKETIO_H
