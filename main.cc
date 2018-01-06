#include <iostream>
#include <string>
#include <vector>

#include <GL/gl3w.h>
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
#include "shader/phong.vert.inc"
};

static const std::string fragment_src = {
#include "shader/phong.frag.inc"
};

int main () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    GLFWwindow * window = glfwCreateWindow(1200, 800, "shading", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

    {
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

        TriangleMesh mFloor;
        mFloor.vert = { {2,2,0}, {-2,2,0}, {-2,-2,0}, {2,-2,0} };
        mFloor.face = { {0,1,2}, {2,3,0} };
        mFloor.calcNorm();

        TriangleMesh mesh;
        if (!mesh.readRaw("data/teapot.raw"))
            cerr << "cannot read file" << endl;

        mesh.calcNorm();

        mFloor.copyToBuffer();
        mesh.copyToBuffer();

        mFloor.bind(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
        mesh.bind(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));

        program.use();
        glCheckError;

        glClearColor(0, .3, .3, 1);

        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glCheckError;
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        while (!glfwWindowShouldClose(window)) {
            {
                int w, h;
                double x, y;
                glfwGetFramebufferSize(window, &w, &h);
                glfwGetCursorPos(window, &x, &y);
                glViewport(0, 0, w, h);
                static float t = 0; t += .01;
                program.uniform("lightDir", glm::normalize(glm::fvec3(.1, x/w-.5, -(y/h-.5))));

                glm::fvec3 eye = glm::fvec3(cosf(t), sinf(t), 0.5) * 3.f;
                program.uniform("eyePos", eye);
                program.uniform("proj", glm::perspective(1.f, 1.f * w / h, 1e-2f, 1e2f) *
                    glm::lookAt(eye, glm::fvec3(0, 0, 0), glm::fvec3(0, 0, 1)));
                glCheckError;
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mFloor.draw();
            mesh.draw();
            glCheckError;
            //glFlush();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
}
