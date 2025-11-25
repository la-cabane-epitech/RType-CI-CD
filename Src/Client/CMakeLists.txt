add_executable(rtype_client
    main.cpp
)

target_include_directories(rtype_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_client PRIVATE raylib)
