#include <iostream>
#include <csignal>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "include/CServer.h"
#include "include/ConfigMgr.h"

int main() {
    std::string gate_port_str = ConfigMgr::getInstance()["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try {
        auto port = static_cast<unsigned short>(gate_port);
        net::io_context ioc{2};
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if(error){
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc,port)->start();
        ioc.run();
    } catch (const std::exception &e) {
        std::cerr << "Exception is: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
