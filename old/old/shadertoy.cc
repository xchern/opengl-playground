#include "GLFWUtil.h"
#include "Shader.h"
#include "Buffer.h"

class ShaderToyApp : public GLFWApp {
    Program program;
    ArrayBuffer abuf;
public:
    ShaderToyApp() {
        Shader vertex(GL_VERTEX_SHADER);
        vertex.loadSource({
                "attribute vec2 pos;"
                "varying vec2 __coord__;"
                "void main() {gl_Position = vec4(pos, 0, 1);}"

                });
        Shader fragment(GL_FRAGMENT_SHADER);
        fragment.loadSource({
                "uniform vec2 iResolution;"
                "uniform float iTime;"
                "uniform float iTimeDelta;"
                "uniform int iFrame;"
                "uniform vec4 iMouse;"
                "uniform vec4 iDate;"
                "uniform float iSampleRate;"
                "varying vec2 __coord__;"
                "void mainImage(out vec4, in vec2);"
                "void main(void) { mainImage(fragColor,__coord * 0.5); }"
                });
        std::vector<Shader> shaders;
        shaders.push_back(std::move(vertex));
        shaders.push_back(std::move(fragment));
        program.link(shaders);
        const static float pos[] = {
            0,0, 1,0, 1,1,
            0,0, 1,1, 0,1
        };
        abuf.data(sizeof(pos), pos, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  
    }
    void draw() override {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
    }
};

int main() {
    GLFWInit glfwInit;
    ShaderToyApp app;
    while(app.loopOnce());
}
