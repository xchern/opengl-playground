#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

using namespace std;
using namespace glm;

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)

static fvec2 points[] = {
    {.5, .5}, {-.5, .5}, {-.5, -.5},
    {-.5, -.5}, {.5, -.5}, {.5, .5},
};

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

    // vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // buffer
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);


    program.use();
    GLuint pos_loc = program.attributeLoc("vPos");
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);
    glEnableVertexAttribArray(pos_loc);
    glCheckError;

    glClearColor(0, .3, .3, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        {
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glViewport(0, 0, w, h);
        }
        {
            static float t = 0; t += .01;
            program.uniform("trans", fvec2({cosf(t), sinf(t)}) * .5f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(points) / sizeof(fvec2));
        glCheckError;
        //glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}
