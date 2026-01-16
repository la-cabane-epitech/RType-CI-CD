set(SOURCES
    main.cpp
    Game.cpp
    ServerManager.cpp
)

add_executable(rtype_server ${SOURCES})

target_link_libraries(rtype_server PRIVATE rtype_network)

install(TARGETS rtype_server DESTINATION ..)