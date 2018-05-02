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
#include <iostream>
#include "Query.h"
#include "SocketAddress.h"
#include "ServerSocket.h"

#ifndef ASSIGNMENT_1_CLIENTSOCKET_H
#define ASSIGNMENT_1_CLIENTSOCKET_H

class ClientSocket {
public:
    static const int BUFFER = 8192;

protected:
    int fd{}, server_fd{};
    socklen_t length{};
    SocketAddress address{};
    char* buffer{};
    size_t buffer_size{};
    bool acceptable{};

    void initialize_buffer(size_t n = BUFFER) {
        if (buffer == nullptr) {
            buffer = new char[n];
        } else if (buffer_size < n) {
            delete buffer;
            buffer = new char[n];
            buffer_size = n;
        }
    }

public:
    explicit ClientSocket(ServerSocket &server, int fd = -1):
            ClientSocket(server.getFd(), fd) {}

    explicit ClientSocket(int server_fd, int fd = -1) {
        if (server_fd == -1) {
            perror("ERROR accepting socket");
            exit(1);
        }

        this->server_fd = server_fd;
        this->fd = fd;

        acceptable = true;
        address.internet = new sockaddr_in();
        length = sizeof(sockaddr_in);
        bzero(address.internet, length);
    }

    bool isAcceptable() {
        return acceptable;
    }

    bool accept() {
        acceptable = false;
        fd = ::accept(server_fd, getGenericAddress(), &length);
        acceptable = true;
        return fd != -1;
    }

    void read(size_t n = BUFFER) {
        initialize_buffer(n + 1);

        FILE *fp;
        long nbytes;

        nbytes = ::read(fd, buffer, n);

        if (nbytes <= 0) {
            return perror("ERROR failed to read the request");
        }

        if (nbytes > 0 && nbytes <= n) {
            buffer[nbytes] = 0;
        } else {
            return read((size_t) nbytes);
        }

        Query query(buffer);
        std::string path = query.get("GET", 0);

        if (path == "/") {
            path = "/index.html";
        }

        if (path.find("../") != -1) {
            path = "/403.html";
        }

        path = "../../public" + path;

        if ((fp = fopen(path.c_str(), "rb")) == nullptr) {
            sprintf(buffer, "HTTP/1.0 404 Not Found\r\nConnection:close\r\n\r\n");
            path = "../../public/404.html";
            fp = fopen(path.c_str(), "rb");
        } else {
            sprintf(buffer, "HTTP/1.0 200 OK\r\nConnection:close\r\n\r\n");
        }

        write(buffer);

        while ((nbytes = fread(buffer, 1, n, fp)) > 0) {
            write(buffer, (size_t) nbytes);
        }

        fclose(fp);

        shutdown();
        printf("Bye, %d\n", getFd());
        close();
    }


    void write(char *response, size_t len) {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::write(fd, response, len) == 1) {
            perror("ERROR sending to socket");
        }
    }

    void write(char *response) {
        write(response, strlen(response));
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

    sockaddr* getGenericAddress() const {
        return address.generic;
    }

    int getFd() const {
        return fd;
    }

    ~ClientSocket() {
        delete[] buffer;
    }
};

#endif //ASSIGNMENT_1_CLIENTSOCKET_H