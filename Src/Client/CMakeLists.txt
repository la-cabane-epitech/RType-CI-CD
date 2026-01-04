cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/Include)

set (
    SOURCES
    main.cpp
    RTypeClient.cpp
    TCPClient.cpp
    UDPClient.cpp
    Renderer.cpp
    ParallaxLayer.cpp
)

add_executable(rtype_client ${SOURCES})

target_include_directories(rtype_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_client PRIVATE raylib asio::asio)