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

#ifndef ASSIGNMENT_1_QUERY_H
#define ASSIGNMENT_1_QUERY_H

class Query {
private:
    std::unordered_map<std::string, std::string> map;
public:
    explicit Query(char *buffer) {
        rebuild(buffer);
    }
    void rebuild(char *buffer) {
        map.clear();

        char *line = strtok(buffer, "\n");

        std::istringstream fin;
        std::string parameter, arguments;

        while (line != nullptr) {
            fin.str(line);
            fin.clear();

            fin >> parameter;
            getline(fin, arguments);

            if (parameter[parameter.size() - 1] == ':') {
                parameter.pop_back();
            }

            size_t i;
            for (i = 0; i < arguments.size() && arguments[i] == ' '; ++i);
            arguments.erase(0, i);

            map[parameter] = arguments;

            line = strtok(nullptr, "\n");
        }
    }
    std::string get(const std::string &key) {
        if (map.count(key)) {
            return map[key];
        }
        return "";
    }
    std::string get(const std::string &key, int token) {
        std::string val = get(key), s;
        std::istringstream str_in(val);

        while(token >= 0 && (str_in >> s)) {
            --token;
        }

        if (token < 0) {
            return s;
        }

        return "";
    }
};

#endif //ASSIGNMENT_1_QUERY_H

