set(SOURCES
    main.cpp
    RTypeClient.cpp
    Renderer.cpp
    ConfigManager.cpp
    ParallaxLayer.cpp
    ClientManager.cpp
)

add_executable(rtype_client ${SOURCES})

target_link_libraries(rtype_client PRIVATE raylib rtype_network)