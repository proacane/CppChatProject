cmake_minimum_required(VERSION 3.28)
project(GateServer)

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find Boost libraries")
endif ()
if (jsoncpp_FOUND)
    include_directories(${jsoncpp_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find Jsoncpp libraries")
endif ()

#set(ADDITIONAL_LIBS ws2_32 wsock32 bcrypt)

# 添加自定义命令，将 config.ini 复制到构建目录
configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)

add_executable(GateServer main.cpp
        include/CServer.h
        src/CServer.cpp
        include/const.h
        include/HttpConnection.h
        src/HttpConnection.cpp
        include/SingleTon.h
        include/LogicSystem.h
        src/LogicSystem.cpp
        proto/message.pb.h
        proto/message.pb.cc
        proto/message.grpc.pb.h
        proto/message.grpc.pb.cc
        include/VerifyGrpcClient.h
        src/VerifyGrpcClient.cpp
        include/ConfigMgr.h
        src/ConfigMgr.cpp
        include/AsioIOServicePool.h
        src/AsioIOServicePool.cpp
        include/RedisMgr.h
        src/RedisMgr.cpp)

target_link_libraries(GateServer PRIVATE ${Boost_LIBRARIES} JsonCpp::JsonCpp gRPC::grpc++ protobuf::libprotobuf hiredis::hiredis spdlog::spdlog)