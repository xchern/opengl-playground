#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "shader.h"
#include "mesh.h"

using namespace std;
using namespace glm;

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)

static const std::string vertex_src = {
#include "shader.vert.inc"
};

static const std::string fragment_src = {
#include "shader.frag.inc"
};

int main () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    GLFWwindow * window = glfwCreateWindow(800, 600, "demo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    Program program;
    {
        vector<Shader> shaders;

        { // vertex
            Shader vertex(GL_VERTEX_SHADER);
            vertex.source(vertex_src);
            vertex.compile();
            if (!vertex.compileStatus())
                cerr << vertex.infoLog() << endl;
            shaders.push_back(move(vertex));
        }

        { // fragment
            Shader fragment(GL_FRAGMENT_SHADER);
            fragment.source(fragment_src);
            fragment.compile();
            if (!fragment.compileStatus())
                cerr << fragment.infoLog() << endl;
            shaders.push_back(move(fragment));
        }

        program.link(shaders);
        if (!program.linkStatus())
            cerr << program.infoLog() << endl;
    }

    TriangleMesh mesh;
    if (!mesh.readRaw("teapot.raw"))
        cerr << "cannot read file" << endl;

    mesh.calcNorm();
    mesh.copyToBuffer();
    // vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    program.use();
    mesh.bind(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
    glCheckError;

    glClearColor(0, .3, .3, 1);

    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        {
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glViewport(0, 0, w, h);
            static float t = 0; t += .01;
            program.uniform("proj", glm::perspective(1.f, 1.f * w / h, 1e-2f, 1e2f) *
                glm::lookAt(glm::fvec3(cosf(t), sinf(t), 1) * 3.f, glm::fvec3(0, 0, 0), glm::fvec3(0, 0, 1)));
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh.draw();
        glCheckError;
        //cerr << program.infoLog() << endl;
        //glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}
