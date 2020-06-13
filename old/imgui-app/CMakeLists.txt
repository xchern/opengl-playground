add_subdirectory(imgui)

include_directories(.)

find_package( OpenGL REQUIRED )
find_package( glfw3 REQUIRED )

add_library(imgui-app
    VideoWriter.cc
    VideoWriter.h
    ImGuiApp.cpp
    ImGuiApp.h
    imgui_impl_opengl3.cpp
    imgui_impl_opengl3.h
    imgui_impl_glfw.cpp
    imgui_impl_glfw.h
    GL/gl3w.c
    GL/gl3w.h
    )
target_link_libraries(imgui-app
    imgui
    glfw
    ${OPENGL_gl_LIBRARY}
    )

add_executable(imgui-demo
    imgui-app-demo.cc
    )
target_link_libraries(imgui-demo
    imgui-app
    )

