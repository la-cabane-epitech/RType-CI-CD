add_library(rtype_network SHARED
    TCPClient.cpp
    UDPClient.cpp
    TCPServer.cpp
    UDPServer.cpp
)

set_target_properties(rtype_network PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_include_directories(rtype_network PUBLIC
    ${CMAKE_SOURCE_DIR}/Include
)

target_link_libraries(rtype_network PUBLIC asio::asio)
if (WIN32)
    target_link_libraries(rtype_network PUBLIC ws2_32 mswsock)
endif()

