/**
 *  FileName: SingleTon.h
 *  CreateTime: 2024/8/13 15:26
 *  Description: 
 *  Author: ACAね
*/
#ifndef GATESERVER_SINGLETON_H
#define GATESERVER_SINGLETON_H

#include <iostream>
#include <memory>

template<typename T>
class SingleTon {
public:
    SingleTon(const SingleTon &) = delete;

    SingleTon &operator=(const SingleTon &) = delete;

    static std::shared_ptr<T> getInstance() {
        static std::shared_ptr<T> _instance(new T);
        return _instance;
    };
//    void PrintAddress() const {
//        std::cout << &getInstance() << std::endl;
//    }
    // 析构函数可以是虚拟的以确保派生类的正确析构
    virtual ~SingleTon() = default;

protected:
    SingleTon() = default;
};

#endif //GATESERVER_SINGLETON_H
