#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)


using namespace std;
using namespace glm;

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
                {0,0,10}, // eye
                {0,0,0}, // center
                {0,1,0}, // up
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
            "shader/texture.frag",
            "shader/texture.vert"
        });
        program.use();
        glCheckError;

        TriangleUVMesh mesh;
        mesh.fromData(
            {{1,1,0}, {0,1,0}, {0,0,0}, {1,0,0}},
            {{0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}},
            {{1,1}, {0,1}, {0,0}, {1,0}},
            {{0,1,2}, {2,3,0}}
        );
        mesh.copyToBuffer();
        mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"), program.attributeLoc("vCoord"));
        //mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
        glCheckError;

        ArrayBuffer bufTrans;
        std::vector<glm::fmat4> trans;
        {
            for (float x = -4; x <= 4; x += 1.2)
                for (float y = -4; y <= 4; y += 1.2)
                    for (float z = 0; z <= 4; z += 1.2) {
                        trans.push_back(glm::translate(glm::fvec3(x, y, z)));
                    }
            int transLoc = program.attributeLoc("trans");
            bufTrans.data(trans.size() * sizeof(glm::fmat4), trans.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(transLoc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::fmat4), (void *)0);
            glVertexAttribPointer(transLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::fmat4), (void *)(4 * sizeof(float)));
            glVertexAttribPointer(transLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::fmat4), (void *)(8 * sizeof(float)));
            glVertexAttribPointer(transLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::fmat4), (void *)(12 * sizeof(float)));
            glEnableVertexAttribArray(transLoc);
            glEnableVertexAttribArray(transLoc + 1);
            glEnableVertexAttribArray(transLoc + 2);
            glEnableVertexAttribArray(transLoc + 3);
            glCheckError;

            glVertexAttribDivisor(transLoc, 1);  
            glVertexAttribDivisor(transLoc + 1, 1);  
            glVertexAttribDivisor(transLoc + 2, 1);  
            glVertexAttribDivisor(transLoc + 3, 1);  
            glCheckError;
        }

        Texture2D texture;
        texture.bind();
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */	
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
        texture.fromFile("data/wall.jpg");
        glActiveTexture(GL_TEXTURE0);
        texture.bind();
        glCheckError;

        glClearColor(0, .3, .3, 1);
        glEnable(GL_CULL_FACE);
        /* glEnable(GL_BLEND); */
        /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */
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
                camera.setResolution(w, h);
                /* program.uniform("lightDir", glm::normalize(glm::fvec3(.1, x/w-.5, -(y/h-.5)))); */
                //program.uniform("lightDir", glm::normalize(glm::fvec3(1, 2, 3)));
                /* program.uniform("eyePos", camera.getEye()); */
                program.uniform("proj", camera.getProjMat());
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mesh.drawInstanced(trans.size());
            glCheckError;
            //glFlush();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
}
