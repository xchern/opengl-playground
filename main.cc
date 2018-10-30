#include "ImGuiApp.h"
#include "camera.h"
#include "gl_helper.h"
#include <math.h>
#include <string>
#include <vector>
#include "joystick.h"
#include "Particles.h"

class App : public ImGui::App {
    VideoWriter vw;
public:
    App(int argc, char ** argv) :
        ImGui::App("ImGuiApp", 1280, 960),
        vw("output.mp4", 60)
    {
        ImGui::GetIO().Fonts->AddFontFromFileTTF("DejaVuSans.ttf", 24.0f);
    }
private:
    Camera cam;
    JoyStick js;
    bool p_control = true;
    bool fullscreen = false;
    virtual void update() override {
        {
            auto & io = ImGui::GetIO();
            int w = io.DisplaySize.x;
            int h = io.DisplaySize.y;
            vw.writeFrameGL(w, h);
        }
        if (js.fetchState()) {
            if (p_control) {
                ImGui::Begin("Joystick Info");
                js.ImGuiShow();
                ImGui::End();
            }
            const glm::vec2 axes_l = glm::vec2(
                    js.getAxis(JoyStick::AXIS_LEFT_X),
                    -js.getAxis(JoyStick::AXIS_LEFT_Y))*ImGui::GetIO().DeltaTime;
            const glm::vec2 axes_r = glm::vec2(
                    js.getAxis(JoyStick::AXIS_RIGHT_X),
                    -js.getAxis(JoyStick::AXIS_RIGHT_Y))*ImGui::GetIO().DeltaTime;
            cam.walk(axes_l);
            cam.rotate(axes_r);
        }
        if (p_control) {
            ImGui::Begin("Camera");
            cam.ImGuiDrag();
            cam.ImGuiEdit();
            ImGui::End();
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
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImDrawList * drawList = ImGui::GetOverlayDrawList();
        drawList->PushClipRectFullScreen();
        const float s = 1.2 + 0.2*cos(ImGui::GetTime()*0.3*2*M_PI);
        drawList->AddCircleFilled(ImGui::GetMousePos(), 20 * sqrt(s), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0,0.5,0.0,1*(1-exp(-1.5/s)))), 36);
        //drawList->AddRect(ImVec2(10,10), 0xffffffff, "nice!");
        drawList->AddText(ImVec2(10,10), 0xffffffff, "nice!");
        drawList->PopClipRect();
        ImGui::Button("");
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}
