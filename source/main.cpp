#include <iostream>
#include "ThreadPool.h"
#include "ServerSocket.h"
#include "ClientSocket.h"

int main() {
    const int POOL_SIZE = 8;
    std::atomic<bool> stop(false);
    auto ref_stop = std::ref(stop);

    std::thread main_thread([&, ref_stop](){
        ServerSocket server;
        server.bind();
        server.listen();

        ClientSocket client = ClientSocket(server);
        ThreadPool pool(POOL_SIZE + 1);

        while(!ref_stop.get()) {
            if (client.accept()) {
                std::cout << "Connected! Hi, " << client.getFd() << "!\n";
                pool.enqueue([client]() mutable {
                    client.read();
                });
            }
        }

        server.shutdown();
        server.close();
    });

    std::string s;
    std::cout << "Type `stop` to stop the server\n";
    while (getline(std::cin, s)) {
        if (s == "stop") {
            std::cout << "Stopping the server...\n";
            stop = true;
            break;
        }
        std::cout << "Unsupported command!\n";
    }

    main_thread.join();

    std::cout << "Bye!\n";

    return 0;
}