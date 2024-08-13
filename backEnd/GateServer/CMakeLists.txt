cmake_minimum_required(VERSION 3.28)
project(GateServer)

find_package(Boost REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
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

add_executable(GateServer main.cpp
        include/CServer.h
        src/CServer.cpp
        include/const.h
        include/HttpConnection.h
        src/HttpConnection.cpp
        include/SingleTon.h
        include/LogicSystem.h
        src/LogicSystem.cpp)

target_link_libraries(GateServer PRIVATE ${Boost_LIBRARIES} JsonCpp::JsonCpp )