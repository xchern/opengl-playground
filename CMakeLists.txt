add_subdirectory(imgui-app)
include_directories(imgui-app)

add_executable(shadertoy
    shadertoy.cc
    )
target_link_libraries(shadertoy
    imgui-app
    )

add_executable(main
    main.cc
    )
target_link_libraries(main
    imgui-app
    )
