#include "ImGuiApp.h"
#include "camera.h"
#include "gl_helper.h"
#include "joystick.h"
#include "Particles.h"
#include "Lines.h"

#include <math.h>
#include <vector>

inline float randFloat() {
    return (float) rand() / RAND_MAX;
}

class App : public ImGui::App {
public:
    App(int argc, char ** argv) : ImGui::App("ParticleShaderProgram", 1280, 960) {
        //glfwSwapInterval(0);
        loadUniform();
        loadParticles();
        loadLines();
    }
    void loadLines() {
        LinesBuffer lb;
        lb.addAABB({-1,-1,-1}, {1,1,1}, {4,4,4});
        lsp.setData(lb.vertexNumber(), lb.posPtr(), lb.colPtr());
    }
private:
    ParticleShaderProgram psp;
    LinesShaderProgram lsp;
    Camera cam;
    JoyStick js;
    bool p_control = true;
    bool fullscreen = false;
    void loadUniform() {
        auto displaySize = ImGui::GetIO().DisplaySize;
        auto mat = cam.getMat(displaySize.x/displaySize.y);
        lsp.setMVP(false, (const float *)&mat);
        psp.setMVP(false, (const float *)&mat);
        psp.setUnitSize(displaySize.y * cam.getDist() / cam.target_size);
    }
    void loadParticles() {
        const size_t N = 1e2;
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
            radius.push_back(0.1 + 0.2 * sqrt(randFloat()));
        }
        psp.setData(N, pos.data(), col.data(), radius.data());
    }
    virtual void update() override {
        // joystick
        if (js.fetchState()) {
            const glm::vec2 axes_l = glm::vec2(
                    js.getAxis(JoyStick::AXIS_LEFT_X),
                    -js.getAxis(JoyStick::AXIS_LEFT_Y))*ImGui::GetIO().DeltaTime;
            const glm::vec2 axes_r = glm::vec2(
                    js.getAxis(JoyStick::AXIS_RIGHT_X),
                    -js.getAxis(JoyStick::AXIS_RIGHT_Y))*ImGui::GetIO().DeltaTime;
            cam.walk(axes_l);
            cam.rotate(axes_r);
        }
        // overlay window
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
        // control window
        if (p_control) {
            ImGui::Begin("control", &p_control);
            if (ImGui::Button("load random particle data")) {
                loadParticles();
            }
            cam.ImGuiDrag();
            cam.ImGuiEdit();
            ImGui::End();
        }
        // draw
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        loadUniform();
        psp.draw();
        glLineWidth(8);
        lsp.draw();
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}
