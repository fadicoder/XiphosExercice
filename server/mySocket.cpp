#include "mySocket.h"

bool MySocket::init(const char* socketPath) {
    char buffer;

    // Create a new client socket with domain: AF_UNIX, type: SOCK_STREAM, protocol: 0
    this->fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (this->fd == -1) {
        std::cerr << "ERROR: Failed to create socket" << std::endl;
        return false;
    }

    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socketPath, sizeof(address.sun_path) - 1);

    return true;
}

bool MySocket::create(const char* socketPath) {
    this->init(socketPath);
    if (bind(fd, (struct sockaddr*) &address, sizeof(struct sockaddr_un)) == -1) {
        std::cerr << "Error: failed to build the socket file" << std::endl;
        return false;
    }
    else {
        std::cout << "INFO: new socket created"<< std::endl;
    }

    // The listen call marks the socket as *passive*. The socket will subsequently
    // be used to accept connections from *active* sockets.
    // listen cannot be called on a connected socket (a socket on which a connect()
    // has been succesfully performed or a socket returned by a call to accept()).
    if (listen(fd, 100) == -1) {
        std::cerr << "Error: Error occured while tying to make socket listen" << std::endl;
        return false;
    }
    return true;
}

bool MySocket::writeData(void* data, ssize_t size) {
    ssize_t nbBytesSent = size;
    ssize_t nbBytesLeft = size;
    while (nbBytesLeft > 0) {
        nbBytesSent = write(fd, data, size);
        nbBytesLeft = size - nbBytesSent;
        if (nbBytesSent == -1) {
            std::cerr << "ERROR: Failed to write data in server socket" << std::endl;
            return false;
        }
    }
    return true;
}

bool MySocket::writeInt(int value) {
    return this->writeData(&value, sizeof(int));
}

bool MySocket::writeFloat(float value) {
    return this->writeData(&value, sizeof(float));
}

bool MySocket::writeChar(char value) {
    return this->writeData(&value, sizeof(char));
}

bool MySocket::writeString(std::string value) {
    if (!this->writeInt(value.size())){
        return false;
    }
    if (!this->writeData((void*) value.c_str(), value.size() * sizeof(char))){
        return false;
    }
    return true;
}

bool MySocket::readData(void* data, ssize_t size) {
    ssize_t nbBytesread = size;
    ssize_t nbBytesLeft = size;
    while (nbBytesLeft > 0) {
        nbBytesread = read(this->fd, data, size);
        nbBytesLeft = size - nbBytesread;
        if (nbBytesread == -1) {
            std::cerr << "ERROR: haha Failed to read data from socket" << std::endl;
            return false;
        }
    }
    return true;
}

bool MySocket::readInt(int* value) {
    return this->readData(value, sizeof(int));
}
bool MySocket::readFloat(float* value) {
    return this->readData(value, sizeof(float));
}
bool MySocket::readChar(char* value){
    return this->readData(value, sizeof(char));
}
bool MySocket::readString(std::string* value) {
    int nbByes;
    if (!this->readInt(&nbByes)){
        return false;
    }
    char charArray[nbByes];
    if (!this->readData(charArray, nbByes * sizeof(char))) {
        return false;
    }
    *value = std::string(charArray, nbByes);
    return true;
}

bool MySocket::acceptClient(void* handleClient(MySocket*)) {
    int cfd = accept(this->fd, NULL, NULL);
    if (cfd == -1) {
        std::cerr << "ERROR: error occurred while trying to accepte a client" << std::endl;
        return false;
    }
    MySocket* clientSocket = new MySocket();
    clientSocket->fd = cfd;
    pthread_t handlerThread;
    const int error = pthread_create(&handlerThread, NULL, (void* (*)(void*)) handleClient, (void*) clientSocket);
    if (error) {
        std::cerr << "ERROR: failed to create thread for client " << cfd << std::endl;
        return false;
    }
    std::cout << "INFO: Client handle completed" << std::endl;
    return true;
}
