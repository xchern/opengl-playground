#include "ImGuiApp.h"
#include "camera.h"
#include "glshader_helper.h"
#include <math.h>
#include <string>
#include <vector>
#include "joystick.h"
#include "Particles.h"

inline float randFloat() {
    return (float) rand() / RAND_MAX;
}

class App : public ImGui::App {
public:
    App(int argc, char ** argv) : ImGui::App("ParticleShaderProgram", 1280, 960) {
        ImGui::GetIO().Fonts->AddFontFromFileTTF("DejaVuSans.ttf", 24.0f);
        loadUniform();
        loadRandomData();
    }
private:
    ParticleShaderProgram program;
    Camera cam;
    JoyStick js;
    bool p_control = true;
    bool fullscreen = false;
    char csvFile[64];
    void loadUniform() {
        auto displaySize = ImGui::GetIO().DisplaySize;
        auto mat = cam.getMat(displaySize.x/displaySize.y);
        program.setMVP(false, (const float *)&mat);
        program.setUnitSize(displaySize.y * cam.getDist() / cam.target_size);
    }
    void loadRandomData() {
        const size_t N = 8e3;
        std::vector<float> pos;
        std::vector<float> col;
        std::vector<float> radius;
        for (int i = 0; i < N; i++) {
            pos.push_back(2 * randFloat() - 1);
            pos.push_back(2 * randFloat() - 1);
            pos.push_back(2 * randFloat() - 1);
            glm::vec3 c = glm::vec3(randFloat(), randFloat(), randFloat());
            c /= glm::max(glm::max(c.r, c.g), c.b);
            c = (c + glm::vec3(1)) / 2.f * 0.7f;
            col.push_back(c.r);
            col.push_back(c.g);
            col.push_back(c.b);
            radius.push_back(0.1 + 0.1 * sqrt(randFloat()));
        }
        program.setData(N, pos.data(), col.data(), radius.data());
    }
    virtual void update() override {
        if (js.fetchState()) {
            js.ImGuiShow();
            const glm::vec2 axes_l = glm::vec2(
                    js.getAxis(JoyStick::AXIS_LEFT_X),
                    -js.getAxis(JoyStick::AXIS_LEFT_Y))*ImGui::GetIO().DeltaTime * 2.f;
            const glm::vec2 axes_r = glm::vec2(
                    js.getAxis(JoyStick::AXIS_RIGHT_X),
                    -js.getAxis(JoyStick::AXIS_RIGHT_Y))*ImGui::GetIO().DeltaTime * 2.f;
            cam.translate(axes_l);
            cam.rotate(axes_r);
        }
        const float dist = 10.f;
        ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - dist, dist);
        ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
        if (ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
            ImGui::Text("framerate: %.1ffps", ImGui::GetIO().Framerate);
            ImGui::Checkbox("show control window", &p_control);
            if (ImGui::Checkbox("fullscreen", &fullscreen))
                setFullScreen(fullscreen);
        }
        ImGui::End();
        if (p_control) {
            ImGui::Begin("control", &p_control);
            if (ImGui::Button("load random data")) {
                loadRandomData();
            }
            //TODO
            /* ImGui::InputText("csv file", csvFile, sizeof(csvFile)); */
            /* if (ImGui::Button("load file")) { */
            /* } */
            cam.ImGuiDrag();
            cam.ImGuiEdit();
            ImGui::End();
        }
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        loadUniform();
        program.draw();
        /* ImDrawList * drawList = ImGui::GetOverlayDrawList(); */
        /* drawList->PushClipRectFullScreen(); */
        /* const float s = 1.2 + 0.2*cos(ImGui::GetTime()*0.3*2*M_PI); */
        /* drawList->AddCircleFilled(ImGui::GetMousePos(), 20 * sqrt(s), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0,0.5,0.0,1*(1-exp(-1.5/s)))), 36); */
        /* drawList->AddText(ImVec2(10,10), 0xffffffff, "nice!"); */
        /* drawList->PopClipRect(); */
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}

