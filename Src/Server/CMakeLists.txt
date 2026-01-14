cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/Include)

set (
    SOURCES
    main.cpp
    TCPServer.cpp
    UDPServer.cpp
    Game.cpp
    ServerManager.cpp
)

add_executable(rtype_server ${SOURCES})

target_include_directories(rtype_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_server PRIVATE asio::asio)

install(TARGETS rtype_server DESTINATION ..)