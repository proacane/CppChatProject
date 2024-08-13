#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
template <typename T>
class SingleTon {
  public:
    SingleTon(const SingleTon&) = delete;
    SingleTon& operator=(const SingleTon&) = delete;
    static std::shared_ptr<T> getInstance() {
        static std::shared_ptr<T> _instance(new T);
        return _instance;
    };

    void PrintAddress() const {
        std::cout << &getInstance() << std::endl;
    }

    // 析构函数可以是虚拟的以确保派生类的正确析构
    virtual ~SingleTon() {
        std::cout << "This is Singleton destructor" << std::endl;
    }

  protected:
    SingleTon() = default;
};

#endif  // SINGLETON_H
