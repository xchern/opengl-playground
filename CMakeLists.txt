cmake_minimum_required(VERSION 2.6)

project(opengl-playground)

add_subdirectory(${CMAKE_SOURCE_DIR}/gl3w)

include_directories(
    ${CMAKE_SOURCE_DIR}/gl3w
    ${CMAKE_SOURCE_DIR}/utils
    )

add_executable(checkShader
    checkShader.cc
    )
target_link_libraries(checkShader
    gl3w
    glfw
    )

add_executable(main
    main.cc
    )
target_link_libraries(main
    gl3w
    glfw
    )