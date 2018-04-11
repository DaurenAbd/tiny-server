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
#include "Query.h"

#ifndef ASSIGNMENT_1_CLIENTSOCKET_H
#define ASSIGNMENT_1_CLIENTSOCKET_H

class ClientSocket {
public:
    static const int BUFFER = 8192;
protected:
    int fd{}, server_fd{};
    socklen_t length{};
    sockaddr_in *address{};
    char *buffer{};
    size_t buffer_size{};

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
    explicit ClientSocket(ServerSocket &server, int fd = -1) : ClientSocket(server.getFd(), fd) {}

    explicit ClientSocket(int server_fd, int fd = -1) {
        if (server_fd == -1) {
            perror("ERROR accepting socket");
            exit(1);
        }
        this->server_fd = server_fd;

        this->fd = fd;
        address = new sockaddr_in();
        length = sizeof(sockaddr_in);
        bzero(address, length);
    }

    bool accept() {
        fd = ::accept(server_fd, getGenericAddress(), &length);
        return fd != -1;
    }

    void read(size_t n = BUFFER) {
        initialize_buffer(n);

        FILE *fp;
        long nbytes;

        nbytes = ::read(fd, buffer, n);

        if (nbytes <= 0) {
            perror("ERROR failed to read the request");
            return;
        }

        if (nbytes > 0 && nbytes < n) {
            buffer[nbytes] = 0;
        } else {
            read((size_t) nbytes + 1);
            return;
        }

        Query query(buffer);
        std::string path = query.get("GET", 0);

        if (path == "/") {
            path = "/index.html";
        }

        if ((fp = fopen(("../../public" + path).c_str(), "rb")) == nullptr) {
            perror("ERROR file not found");
            exit(1);
        }

        sprintf(buffer, "HTTP/1.0 200 OK\r\nConnection:close\r\n\r\n");
        write(buffer);

        while ((nbytes = fread(buffer, 1, n, fp)) > 0) {
            write(buffer, (size_t) nbytes);
        }

        fclose(fp);
        shutdown();

        while(::read(fd, buffer, n) > 0);

        std::cout << "Bye, " << getFd() << "\n";
        close();
    }


    void write(char *response, size_t len) {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::write(fd, response, len) == 1) {
            perror("ERROR sending to socket");
            exit(1);
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
            exit(1);
        }
    }

    void close() {
        if (fd == -1) {
            perror("ERROR accessing closed socket");
            exit(1);
        }
        if (::close(fd)) {
            perror("ERROR closing socket");
            exit(1);
        }
        fd = -1;
    }

    sockaddr *getGenericAddress() const {
        return (sockaddr *) address;
    }

    int getFd() const {
        return fd;
    }
};

#endif //ASSIGNMENT_1_CLIENTSOCKET_H