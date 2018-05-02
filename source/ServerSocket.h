//
// Created by daurenabd on 18. 4. 10.
//

#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <algorithm>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <climits>
#include <sstream>
#include <unordered_map>
#include <map>
#include "SocketAddress.h"

#ifndef ASSIGNMENT_1_SERVERSOCKET_H
#define ASSIGNMENT_1_SERVERSOCKET_H


class ServerSocket {
public:
    static const uint16_t PORT = 5000;
    static const int QUEUE = 3;

protected:
    int fd;
    socklen_t length;
    SocketAddress address{};

public:
    explicit ServerSocket(const char *ip = nullptr, uint16_t port = PORT) {
        this -> fd = socket(AF_INET, SOCK_STREAM, 0);

        if (this -> fd == -1) {
            perror("ERROR opening socket");
            exit(1);
        }

        int opt_val = 1;
        timeval timeout{};
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt(this -> fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            perror("ERROR setting socket options");
            exit(1);
        }

        if (setsockopt(this -> fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            perror("ERROR setting socket options");
            exit(1);
        }

        if (setsockopt(this -> fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int))) {
            perror("ERROR setting socket options");
            exit(1);
        }

        address.internet = new sockaddr_in();
        length = sizeof(sockaddr_in);
        bzero(address.internet, length);
        address.internet->sin_family = AF_INET;
        address.internet->sin_addr.s_addr = (ip == nullptr) ? INADDR_ANY : inet_addr(ip);
        address.internet->sin_port = htons(port);
    }

    int bind() {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::bind(fd, getGenericAddress(), length)) {
            perror("ERROR binding socket");
        }
    }

    int listen(int n = QUEUE) {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        ::listen(fd, n);
    }

    void shutdown() {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::shutdown(fd, SHUT_RDWR)) {
            perror("ERROR shutting down socket");
        }
    }

    void close() {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::close(fd)) {
            perror("ERROR closing socket");
        }
        fd = -1;
    }

    int getFd() const {
        return fd;
    }

    sockaddr* getGenericAddress() const {
        return address.generic;
    }
};

#endif //ASSIGNMENT_1_SERVERSOCKET_H
