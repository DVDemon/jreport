cmake_minimum_required(VERSION 3.10)

project(jserver C CXX)

set(EXECUTABLE jserver)
set(EXECUTABLE_POCO jserver_poco)
set(EXPORT_EXECUTABLE xlsx_report)
set(FIX_EXECUTABLE fix_product)
set(PRODUCTS_EXECUTABLE export_products)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL MongoDB)
find_package(Boost COMPONENTS filesystem system program_options regex REQUIRED)

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

add_executable(${EXECUTABLE_POCO} main_poco.cpp
                             config/config.cpp
                             database/database.cpp
                             database/cache.cpp
                             model/issue.cpp
                             model/cluster.cpp
                             model/initiative.cpp
                             model/product.cpp
                             model/comment.cpp
                             model/product_project.cpp
                             model/cluster_project.cpp
                             model/cluster_initiative_issue.cpp
                             model/product_initiative_issue.cpp
                             loader/jira_loader.cpp)

add_executable(${PRODUCTS_EXECUTABLE} export_products.cpp
                             config/config.cpp
                             database/database.cpp
                             database/cache.cpp
                             model/issue.cpp
                             model/cluster.cpp
                             model/initiative.cpp
                             model/product.cpp
                             model/comment.cpp
                             model/product_project.cpp
                             model/cluster_project.cpp
                             model/cluster_initiative_issue.cpp
                             model/product_initiative_issue.cpp
                             loader/jira_loader.cpp)

add_executable(${EXECUTABLE} main.cpp
                             config/config.cpp
                             database/database.cpp
                             database/cache.cpp
                             model/issue.cpp
                             model/cluster.cpp
                             model/initiative.cpp
                             model/product.cpp
                             model/comment.cpp
                             model/product_project.cpp
                             model/cluster_project.cpp
                             model/cluster_initiative_issue.cpp
                             model/product_initiative_issue.cpp
                             loader/jira_loader.cpp)

add_executable(${EXPORT_EXECUTABLE} xlsx_export.cpp
                                    config/config.cpp
                                    database/database.cpp
                                    database/cache.cpp
                                    model/issue.cpp
                                    model/cluster.cpp
                                    model/initiative.cpp
                                    model/product.cpp
                                    model/comment.cpp
                                    model/product_project.cpp
                                    model/cluster_project.cpp
                                    model/cluster_initiative_issue.cpp
                                    model/product_initiative_issue.cpp
                                    file_export/xls_export.cpp
                                    loader/jira_loader.cpp)

add_executable(${FIX_EXECUTABLE}    fix_product.cpp
                                    config/config.cpp
                                    database/database.cpp
                                    database/cache.cpp
                                    model/issue.cpp
                                    model/cluster.cpp
                                    model/initiative.cpp
                                    model/product.cpp
                                    model/comment.cpp
                                    model/product_project.cpp
                                    model/cluster_project.cpp
                                    model/cluster_initiative_issue.cpp
                                    model/product_initiative_issue.cpp
                                    loader/jira_loader.cpp)

target_link_libraries(${EXECUTABLE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "OpenXLSX"
                             stdc++fs
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)

target_link_libraries(${EXECUTABLE_POCO} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "OpenXLSX"
                             stdc++fs
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)

target_link_libraries(${PRODUCTS_EXECUTABLE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "OpenXLSX"
                             stdc++fs
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)

target_link_libraries(${EXPORT_EXECUTABLE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "OpenXLSX"
                             stdc++fs
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)

target_link_libraries(${FIX_EXECUTABLE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "OpenXLSX"
                             stdc++fs
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             OpenSSL::SSL
                             OpenSSL::Crypto)