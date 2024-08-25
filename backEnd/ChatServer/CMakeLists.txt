cmake_minimum_required(VERSION 3.28)
project(ChatServer)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
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

# 添加自定义命令，将 config.ini 复制到构建目录
configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)


set(MYSQL_CONNECTOR_CPP_INCLUDE_DIR "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\include")  # 替换为实际的安装路径
set(MYSQL_CONNECTOR_CPP_LIBRARY_DIR "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\lib64\\debug\\vs14")  # 替换为实际的安装路径
# 添加头文件路径
include_directories(${MYSQL_CONNECTOR_CPP_INCLUDE_DIR})
# 添加库文件路径
link_directories(${MYSQL_CONNECTOR_CPP_LIBRARY_DIR})


add_executable(ChatServer main.cpp
        include/ConfigMgr.h
        src/ConfigMgr.cpp
        include/CServer.h
        src/CServer.cpp
        include/CSession.h
        src/CSession.cpp
        include/SingleTon.h
        include/AsioIOServicePool.h
        src/AsioIOServicePool.cpp
        include/const.h
        include/MsgNode.h
        src/MsgNode.cpp
        include/LogicSystem.h
        src/LogicSystem.cpp
        include/MysqlMgr.h
        include/MysqlDao.h
        src/MysqlMgr.cpp
        src/MysqlDao.cpp
        proto/message.pb.cc
        proto/message.grpc.pb.cc
        proto/message.grpc.pb.h
        proto/message.pb.h
        proto/message.proto
        include/data.h
include/StatusGrpcClient.h
src/StatusGrpcClient.cpp)

target_link_libraries(ChatServer PRIVATE ${Boost_LIBRARIES} JsonCpp::JsonCpp gRPC::grpc++ protobuf::libprotobuf hiredis::hiredis spdlog::spdlog mysqlcppconn)