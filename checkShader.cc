#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"

using namespace std;

/* help of glslangValidator
'file' can end in .<stage> for auto-stage classification, where <stage> is:
    .conf   to provide a config file that replaces the default configuration
            (see -c option below for generating a template)
    .vert   for a vertex shader
    .tesc   for a tessellation control shader
    .tese   for a tessellation evaluation shader
    .geom   for a geometry shader
    .frag   for a fragment shader
    .comp   for a compute shader
*/

int main (int argc, const char ** argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    GLFWwindow * window = glfwCreateWindow(8, 8, "OpenGLContext", NULL, NULL);
    glfwHideWindow(window);
    glfwMakeContextCurrent(window);
    gl3wInit();

    Program prog;
    prog.loadFiles(std::vector<std::string>(argv + 1, argv + argc));

    glfwTerminate();
}
