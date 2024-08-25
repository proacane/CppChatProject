/**
 *  FileName: AsioIOServicePool.cpp
 *  CreateTime: 2024/8/24 14:18
 *  Description: 
 *  Author: ACAね
*/
#include <memory>
#include <spdlog/spdlog.h>
#include "../include/AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(size_t size) : _ioServices(size), _works(size), _next_IOService(0) {
    for (size_t i = 0; i < size; i++) {
        _works[i] = std::make_unique<work>(_ioServices[i]);
    }

    for (size_t i = 0; i < size; i++) {
        _threads.emplace_back([this, i] {
            _ioServices[i].run();
        });
    }

}

AsioIOServicePool::~AsioIOServicePool() {
    stop();
    spdlog::info("AsioIOServicePool destructor");
}


boost::asio::io_context &AsioIOServicePool::getIOService() {
    auto &service = _ioServices[_next_IOService];
    // 轮询策略返回 io_context 实例
    _next_IOService = (_next_IOService + 1) % _ioServices.size();
    return service;
}

void AsioIOServicePool::stop() {
    for (auto &work: _works) {
        work->get_io_context().stop();
        work.reset();
    }
    for (auto &thread: _threads) {
        thread.join();
    }
}
