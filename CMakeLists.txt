cmake_minimum_required(VERSION 3.10)

project(jserver C CXX)

set(EXECUTABLE jserver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(Boost COMPONENTS filesystem system program_options regex REQUIRED)

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

add_executable(${EXECUTABLE} main.cpp
                             config/config.cpp
                             database/database.cpp)
target_compile_options(${EXECUTABLE} PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(${EXECUTABLE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)
