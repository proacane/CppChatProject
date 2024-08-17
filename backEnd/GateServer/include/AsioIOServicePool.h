/**
 *  FileName: AsioIOServicePool.h
 *  CreateTime: 2024/8/17 13:23
 *  Description: ASIO 线程池
 *  Author: ACAね
*/
#ifndef GATESERVER_ASIOIOSERVICEPOOL_H
#define GATESERVER_ASIOIOSERVICEPOOL_H

#include "SingleTon.h"
#include <boost/asio.hpp>
#include <vector>

class AsioIOServicePool : public SingleTon<AsioIOServicePool> {
    using IOService = boost::asio::io_context;
    using work = boost::asio::io_context::work;
    using workPtr = std::unique_ptr<work>;

    friend class SingleTon<AsioIOServicePool>;

public:
    ~AsioIOServicePool() override;

    AsioIOServicePool(const AsioIOServicePool &) = delete;

    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

    IOService & getIOService() ;
    // 停止所有线程
    void stop();
private:
    /**
     * 参数为要创建的线程数
     * @param size
     */
    explicit AsioIOServicePool(std::size_t size = 2);
    // 要取出的 io_context 索引
    std::size_t _next_ioService;
    // 存储 io_context
    std::vector<IOService> _ioServices;
    // 存储工作线程
    std::vector<workPtr> _workers;
    // 线程
    std::vector<std::thread> _threads;
};


#endif //GATESERVER_ASIOIOSERVICEPOOL_H
