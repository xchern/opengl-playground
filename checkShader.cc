#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

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
    bool success = true;

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

    // reading & compiling files
    vector<Shader> shaders;
    for (const char ** namep = argv + 1; namep < argv + argc; namep++) {
        const std::string filename(*namep);
        const std::string ext(filename.substr(filename.size() - 5, 5));

        ifstream ifs(filename);
        if (!ifs.is_open()) {
            cerr << "cannot open: " << filename << endl;
            exit(1);
        }
        string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        GLenum type;
        if (ext == ".vert") type = GL_VERTEX_SHADER;
        if (ext == ".frag") type = GL_FRAGMENT_SHADER;
        Shader s(type);
        s.source(content);
        s.compile();
        if (!s.compileStatus()) {
            cerr << "error compiling '" << filename << "':" << endl
                 << s.infoLog() << endl;
            success = false;
        }
        shaders.push_back(move(s));
    }

    if (!success) return 1;

    // try linking
    {
        Program program;
        program.link(shaders);
        if (!program.linkStatus()) {
            cerr << "error linking program:" << endl
                 << program.infoLog() << endl;
        }
    }

    if (!success) return 1;

    glfwTerminate();
}