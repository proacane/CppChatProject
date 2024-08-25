/**
 *  FileName: AsioIOServicePool.cpp
 *  CreateTime: 2024/8/17 13:29
 *  Description: 
 *  Author: ACAね
*/
#include <memory>

#include "../include/AsioIOServicePool.h"
#include <spdlog/spdlog.h>

AsioIOServicePool::~AsioIOServicePool() {
    stop();
    spdlog::info("AsioIOServicePool destruct");
}

AsioIOServicePool::IOService &AsioIOServicePool::getIOService() {
    auto &service = _ioServices[_next_ioService];
    // 轮询策略返回 io_context 实例
    _next_ioService = (_next_ioService + 1) % _ioServices.size();
    return service;
}

void AsioIOServicePool::stop() {
    for (auto &work: _workers) {
        // 先停止服务  auto &service = _ioServices[_next_ioService];
        //    // 轮询策略返回 io_context 实例
        //    _next_ioService = (_next_ioService + 1) % _ioServices.size();
        //    return service;
        work->get_io_context().stop();
        // 再清除 work 对象
        work.reset();
    }
    // 让所有线程执行完毕
    for (auto &t: _threads) {
        t.join();
    }
}

AsioIOServicePool::AsioIOServicePool(std::size_t size) : _ioServices(size), _workers(size), _threads(size),
                                                         _next_ioService(0) {
    // 创建并注册 worker
    for (std::size_t i = 0; i < size; i++) {
        _workers[i] = std::make_unique<work>(_ioServices[i]);
    }
    // 启动线程
    for (std::size_t i = 0; i < size; i++) {
        _threads.emplace_back([this, i] {
            _ioServices[i].run();
        });
    }
}
