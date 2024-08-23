cmake_minimum_required(VERSION 3.28)
project(StatusServer)

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

set(MYSQL_CONNECTOR_CPP_INCLUDE_DIR "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\include")  # 替换为实际的安装路径
set(MYSQL_CONNECTOR_CPP_LIBRARY_DIR "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\lib64\\debug\\vs14")  # 替换为实际的安装路径
# 添加头文件路径
include_directories(${MYSQL_CONNECTOR_CPP_INCLUDE_DIR})
# 添加库文件路径
link_directories(${MYSQL_CONNECTOR_CPP_LIBRARY_DIR})


add_executable(StatusServer main.cpp
        proto/message.pb.cc
        proto/message.grpc.pb.cc
        proto/message.grpc.pb.h
        proto/message.pb.h
        proto/message.proto
        include/ConfigMgr.h
        include/const.h
        include/MysqlDao.h
        include/MysqlMgr.h
        include/RedisMgr.h
        include/AsioIOServicePool.h
        include/SingleTon.h
        src/ConfigMgr.cpp
        src/MysqlDao.cpp
        src/MysqlMgr.cpp
        src/RedisMgr.cpp
        src/AsioIOServicePool.cpp
        include/StatusServiceImpl.h
        src/StatusServiceImpl.cpp)
add_custom_command(TARGET StatusServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\lib64\\debug\\mysqlcppconn-10-vs14.dll"
        $<TARGET_FILE_DIR:StatusServer>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:\\develop_tools\\cppsoft\\mysql-connector-c++-9.0.0-winx64\\lib64\\debug\\mysqlcppconnx-2-vs14.dll"
        $<TARGET_FILE_DIR:StatusServer>
        COMMENT "Copying MySQL Connector/C++ DLLs to the output directory"
)
target_link_libraries(StatusServer PRIVATE ${Boost_LIBRARIES} JsonCpp::JsonCpp gRPC::grpc++ protobuf::libprotobuf hiredis::hiredis spdlog::spdlog mysqlcppconn)
