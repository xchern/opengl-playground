#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "GLFWUtil.h"

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)


using namespace std;
using namespace glm;

class WalkApp : public GLFWApp {
protected:
    Camera camera;
    WalkApp() : camera ( {0,0,10}, // eye
            {0,0,0}, // center
            {0,1,0}, // up
            0.5, // fovy
            1080, 720 // resolution
           ) {}
    enum { NORMAL, ROTATE, GRAB } manipState = NORMAL;
    void key_callback(int key, int scancode, int action, int mods) override {
        switch (key) {
            case GLFW_KEY_W: camera.walk(0.05,0,0); break;
            case GLFW_KEY_S: camera.walk(-0.05,0,0); break;
            case GLFW_KEY_D: camera.walk(0,0.05,0); break;
            case GLFW_KEY_A: camera.walk(0,-0.05,0); break;
            case GLFW_KEY_SPACE: camera.walk(0,0,0.05); break;
            case GLFW_KEY_TAB: camera.walk(0,0,-0.05); break;

            case GLFW_KEY_Q: exit(0);
        }
    }
    double x, y; // previous x and y position
    void cursor_position_callback(double xpos, double ypos) override {
        double dx = (xpos - x) / camera.height, dy = (ypos - y) / camera.height;
        x = xpos; y = ypos;
        switch (manipState){
            case NORMAL:
                break;
            case ROTATE:
                camera.rotate(-dx * camera.getFovy(), dy * camera.getFovy());
                break;
            case GRAB:
                camera.walk(0, -dx * camera.getDistance() * camera.getFovy(),
                        dy * camera.getDistance() * camera.getFovy());
                break;
        }
    }
    void mouse_button_callback(int button, int action, int mods) override {
        if (action == GLFW_RELEASE) {
            manipState = NORMAL;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    manipState = ROTATE;
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    manipState = GRAB;
                    break;
            }
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
};

class App : public WalkApp {
    Program program;
    TriangleUVMesh mesh;
    ArrayBuffer bufTrans;
    std::vector<glm::fmat4> trans;
    Texture2D texture;
public:
    App() {
        program.loadFiles({
            "shader/texture.frag",
            "shader/texture.vert"
        });
        program.use();
        glCheckError;

        mesh.loadData(
            {{1,1,0}, {0,1,0}, {0,0,0}, {1,0,0}},
            {{0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}},
            {{1,1}, {0,1}, {0,0}, {1,0}},
            {{0,1,2}, {2,3,0}}
        );
        mesh.copyToBuffer();
        mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"), program.attributeLoc("vCoord"));
        //mesh.bindVA(program.attributeLoc("vPos"), program.attributeLoc("vNorm"));
        glCheckError;

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

        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */	
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
        /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
        texture.loadFile("data/wall.jpg");
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
    }
    void draw() override {
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
    }
};

int main () {
    GLFWInit glfwInit;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    App app;
    while(app.loopOnce());
}
