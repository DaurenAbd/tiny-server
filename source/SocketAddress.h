//
// Created by daurenabd on 18. 5. 2.
//

#ifndef ASSIGNMENT_1_UTILS_H
#define ASSIGNMENT_1_UTILS_H

#include <netinet/in.h>

union SocketAddress {
    sockaddr_in* internet;
    sockaddr* generic;
};

#endif //ASSIGNMENT_1_UTILS_H
