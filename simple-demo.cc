#include <ImGuiApp.h>
#include "simple.h"
#include "camera.h"

using namespace glm;

class App : public ImGui::App {
    SimpleShaderProgram ssp;
    SimpleVertexList svl;
    Camera cam;
public:
    App(int argc, char ** argv) : ImGui::App("ImGuiApp", 1280, 960) {
        ssp.use();
        svl.addAABB({-0.1,-0.1,-0.1}, {0.1,0.1,0.1});
        svl.addAxes({0,0,0}, 1);
        svl.bufferData(GL_STATIC_DRAW);
        glClearColor(0.8,0.8,0.8,1);
        glEnable(GL_DEPTH_TEST);
    }
    virtual void update() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.ImGuiDrag();
        cam.ImGuiEdit();
        {
            auto displaySize = ImGui::GetIO().DisplaySize;
            auto mat = cam.getMat(displaySize.x/displaySize.y);
            ssp.setMVP(false, (const float *)&mat);
        }
        {
            ssp.use();
            glBindVertexArray(svl.getVAO());
            glDrawArrays(GL_LINES, 0, svl.vertexNumber());
            glCheckError();
        }
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}
