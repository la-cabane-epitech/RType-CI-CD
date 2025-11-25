add_executable(rtype_server
    main.cpp
    Server.cpp
    # PacketManager.cpp
)

target_include_directories(rtype_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_server PRIVATE raylib)
