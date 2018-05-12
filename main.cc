#include <iostream>
#include <string>
#include <vector>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

using namespace std;
using namespace glm;

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)

static Program getProgram() {
    static const std::string vertex_src = {
#include "shader/smooth.vert.inc"
    };

    static const std::string fragment_src = {
#include "shader/smooth.frag.inc"
    };

    static const std::string brdf_src = {
#include "shader/phong.frag.inc"
    };

    Program program;
    {
        vector<Shader> shaders;

        { // vertex
            Shader vertex(GL_VERTEX_SHADER);
            vertex.source(vertex_src);
            vertex.compile();
            shaders.push_back(move(vertex));
        }

        { // fragment
            Shader fragment(GL_FRAGMENT_SHADER);
            fragment.source(fragment_src);
            fragment.compile();
            shaders.push_back(move(fragment));
        }

        { // fragment brdf
            Shader fragment(GL_FRAGMENT_SHADER);
            fragment.source(brdf_src);
            fragment.compile();
            shaders.push_back(move(fragment));
        }

        program.link(shaders);
    }
    return program;
}

int main () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    GLFWwindow * window = glfwCreateWindow(1080, 720, "shading", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

    glfwSetKeyCallback(window, [](GLFWwindow * window, int key, int scancode, int action, int mode){
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) exit(0);
    });

    {
        Program program = getProgram();
        program.use();

        SmoothTriangleMesh mFloor;
        mFloor.fromData(
                {{1,1,0}, {-1,1,0}, {-1,-1,0}, {1,-1,0}},
                {{0,1,2}, {2,3,0}}
        );

        FlatTriangleMesh mesh;
        {
            SimpleMesh m;
            m.readRaw("data/ball.raw");
            mesh.fromData(m.vertPos, m.face);
        }

        mFloor.copyToBuffer();
        mesh.copyToBuffer();

        glCheckError;

        mFloor.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
        mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));

        glCheckError;

        glClearColor(0, .3, .3, 1);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glCheckError;
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        while (!glfwWindowShouldClose(window)) {
            glm::mat4 proj;
            {
                int w, h;
                double x, y;
                glfwGetFramebufferSize(window, &w, &h);
                glfwGetCursorPos(window, &x, &y);
                glViewport(0, 0, w, h);
                //program.uniform("lightDir", glm::normalize(glm::fvec3(.1, x/w-.5, -(y/h-.5))));
                program.uniform("lightDir", glm::normalize(glm::fvec3(1, 2, 3)));

                static float t = 0; t += .001;
                //glm::fvec3 eye = glm::fvec3(cosf(t), sinf(t), 1.f) * 10.f;
                float theta = (x / w - .5) * ((float)w/h) * 2;
                glm::fvec3 eye = glm::normalize(-glm::fvec3(cos(theta), sin(-theta), -(y / h - .5))) * 10.f;
                program.uniform("eyePos", eye);
                program.uniform("proj", glm::perspective(1.0f, 1.f * w / h, 1e-2f, 1e2f) *
                                 glm::lookAt(eye, glm::fvec3(0, 0, 0), glm::fvec3(0, 0, 1)));
                glCheckError;
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            program.uniform("trans", glm::translate(glm::fvec3(0, 0, -4.01)) * glm::scale(glm::fvec3(10)));
            mFloor.draw();
            for (float x = -6; x <= 6; x += 4)
                for (float y = -6; y <= 6; y += 4)
                    for (float z = 0; z <= 4; z += 4) {
                        program.uniform("trans",
                                        glm::translate(glm::fvec3(x, y, z)));
                        mesh.draw();
                    }
            glCheckError;
            //glFlush();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
}
