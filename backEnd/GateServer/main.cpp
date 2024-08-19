#include <iostream>
#include <csignal>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "include/CServer.h"
#include "include/ConfigMgr.h"
#include "include/RedisMgr.h"
void TestRedisMgr() {
    assert(RedisMgr::getInstance()->set("blogwebsite","llfc.club"));
    std::string value="";
    assert(RedisMgr::getInstance()->get("blogwebsite", value) );
    assert(RedisMgr::getInstance()->get("nonekey", value) == false);
    assert(RedisMgr::getInstance()->hSet("bloginfo","blogwebsite", "llfc.club"));
    std::string v  = "blogwebsite";
    assert(RedisMgr::getInstance()->hGet("bloginfo",v) != "");
    assert(RedisMgr::getInstance()->existsKey("bloginfo"));
    assert(RedisMgr::getInstance()->del("bloginfo"));
    assert(RedisMgr::getInstance()->del("bloginfo"));
    assert(RedisMgr::getInstance()->existsKey("bloginfo") == false);
    assert(RedisMgr::getInstance()->lPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::getInstance()->lPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::getInstance()->lPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::getInstance()->rPop("lpushkey1", value));
    assert(RedisMgr::getInstance()->rPop("lpushkey1", value));
    assert(RedisMgr::getInstance()->lPop("lpushkey1", value));
    assert(RedisMgr::getInstance()->lPop("lpushkey2", value)==false);
    RedisMgr::getInstance()->close();
}
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
//TestRedisMgr();
    return 0;
}
