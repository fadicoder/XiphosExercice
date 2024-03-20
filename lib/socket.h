#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "error.h"
#include "socketError.h"

class Socket {
private:
    int fd;

public:
    Socket();
    Socket(int fd);
    ~Socket();

    SocketError connectToServer(const char* socketPath);

    bool writeData(void* data, ssize_t size);
    bool writeInt(int value);
    bool writeFloat(float value);
    bool writeChar(char value);
    bool writeString(std::string value);

    bool readData(void* data, ssize_t size);
    bool readInt(int* value);
    bool readFloat(float* value);
    bool readChar(char* value);
    bool readString(std::string* value);
};

#endif // SOCKET_H