#pragma once

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#define glCheckError()                                                      \
    do {                                                                    \
        GLenum err = glGetError();                                          \
        if (err != GL_NO_ERROR)                                             \
            fprintf(stderr,                                                 \
                    "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                               \
    } while (0)

namespace ImGui {
class App {
public:
    App(const char * title=NULL, int width = 1080, int height = 720);
    ~App();
    int exec();
protected:
    virtual void update() = 0;
private:
    GLFWwindow * window;
};
}
