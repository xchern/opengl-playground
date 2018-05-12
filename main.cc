#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

using namespace std;
using namespace glm;

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)

static std::function<void(int key)> kb_cb;

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
            if (action != GLFW_RELEASE) kb_cb(key);
    });

    {
        Camera camera(
                {10,0,0}, // eye
                {0,0,0}, // center
                {0,0,1}, // up
                0.5, // fovy
                1080, 720 // resolution
                );
        kb_cb = [&](int key) {
            switch(key) {
                case GLFW_KEY_L: camera.rotate(0.05, 0); break;
                case GLFW_KEY_J: camera.rotate(-0.05, 0); break;
                case GLFW_KEY_I: camera.rotate(0, 0.05); break;
                case GLFW_KEY_K: camera.rotate(0, -0.05); break;

                case GLFW_KEY_W: camera.walk(0.05,0,0); break;
                case GLFW_KEY_S: camera.walk(-0.05,0,0); break;
                case GLFW_KEY_D: camera.walk(0,0.05,0); break;
                case GLFW_KEY_A: camera.walk(0,-0.05,0); break;
                case GLFW_KEY_SPACE: camera.walk(0,0,0.05); break;
                case GLFW_KEY_TAB: camera.walk(0,0,-0.05); break;

                case GLFW_KEY_Q: exit(0);
            }
        };

        Program program;
        program.fromFiles({
            "shader/smooth.vert",
            "shader/smooth.frag",
            "shader/phong.frag"
        });
        program.use();

        SmoothTriangleMesh mFloor;
        mFloor.fromData(
            {{1,1,0}, {-1,1,0}, {-1,-1,0}, {1,-1,0}},
            {{0,1,2}, {2,3,0}}
        );

        SmoothTriangleMesh mesh;
        {
            SimpleMesh m;
            m.readRaw("data/coke.raw");
            mesh.fromData(m.vertice, m.face);
        }

        mFloor.copyToBuffer();
        mesh.copyToBuffer();

        mFloor.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
        mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));

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
                program.uniform("eyePos", camera.getEye());
                program.uniform("proj", camera.getProjMat());
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            program.uniform("trans", glm::translate(glm::fvec3(0, 0, -4.00)) * glm::scale(glm::fvec3(10)));
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
