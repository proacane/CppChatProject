/**
 *  FileName: AsioIOServicePool.h
 *  CreateTime: 2024/8/24 14:18
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_ASIOIOSERVICEPOOL_H
#define CHATSERVER_ASIOIOSERVICEPOOL_H

#include "SingleTon.h"
#include <vector>
#include <boost/asio.hpp>

class AsioIOServicePool : public SingleTon<AsioIOServicePool> {
    friend class SingleTon<AsioIOServicePool>;

    using work = boost::asio::io_context::work;
    using workPtr = std::unique_ptr<work>;

public:
    ~AsioIOServicePool() override;

    boost::asio::io_context &getIOService();

    AsioIOServicePool(const AsioIOServicePool &) = delete;

    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

    void stop();
private:
    explicit AsioIOServicePool(size_t size = std::thread::hardware_concurrency());

    std::vector<boost::asio::io_context> _ioServices;
    std::vector<workPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _next_IOService;
};

#endif //CHATSERVER_ASIOIOSERVICEPOOL_H
