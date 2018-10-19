include_directories(.)
include_directories(imgui)

add_library(imgui
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    )

add_library(imgui-app
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
    GL
    glfw
    dl
    )

add_executable(imgui-demo
    imgui-app-demo.cc
    )
target_link_libraries(imgui-demo
    imgui-app
    )

