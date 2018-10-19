#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

struct Camera {
    glm::vec3 eye, centor, up;
    float target_size;
    Camera() : eye(0,3,0), centor(0,0,0), up(0,0,1), target_size(1) {}
    float getFovy() const {
        return 2 * atan(target_size/length(eye-centor));
    }
    glm::mat4 getMat(float ratio) const {
        const float dist = length(eye-centor);
        return glm::perspective(getFovy(), ratio, 5e-2f * dist, 1e3f * dist)
            * glm::lookAt(eye, centor, up);
    }
    void mouseRotate(glm::vec2 delta) {
        delta *= getFovy();
        glm::vec3 dr = eye - centor;
        glm::vec3 right = cross(normalize(up), normalize(dr));
        const double c = dot(normalize(up), normalize(dr));
        const double s = sqrt(1 - c * c);
        dr = rotate(dr, -delta.x, up);
        if (c < 0 && delta.y > s) delta.y = s - 1e-2;
        if (c >= 0 && delta.y < -s) delta.y = -s + 1e-2;
        dr = rotate(dr, delta.y, right);
        eye = centor + dr;
    }
    void mouseTranslate(glm::vec2 delta) {
        glm::vec3 dr = centor - eye;
        glm::vec3 right = normalize(cross(dr, up));
        glm::vec3 up = normalize(cross(right, dr));
        eye -= (delta.x * right + delta.y * up) * target_size;
        centor -= (delta.x * right + delta.y * up) * target_size;
    }
    void mouseScale(glm::vec2 delta) {
        delta *= 2;
        target_size *= 1 + delta.y;
        glm::vec3 dr = eye - centor;
        dr *= 1 + delta.x;
        eye = centor + dr;
    }
    void ImGuiEdit() {
        ImGui::InputFloat3("eye", (float*)&eye, "%g");
        ImGui::InputFloat3("centor", (float*)&centor, "%g");
        ImGui::InputFloat3("up", (float*)&up, "%g");
        ImGui::InputFloat("target size", (float*)&target_size, 0,0,"%g");
        target_size = glm::max(0.f, target_size);
    }
    void ImGuiDrag() {
        auto & io = ImGui::GetIO();
        auto delta = glm::vec2(io.MouseDelta.x, -io.MouseDelta.y) / io.DisplaySize.y;
        ImGui::Text("Click & Drag:");
        ImGui::SameLine();
        ImGui::Button("Rotate");
        if (ImGui::IsItemActive()&&ImGui::IsMouseDragging()) {
            mouseRotate(delta);
        }
        ImGui::SameLine();
        ImGui::Button("Translate");
        if (ImGui::IsItemActive()&&ImGui::IsMouseDragging()) {
            mouseTranslate(delta);
        }
        ImGui::SameLine();
        ImGui::Button("Scale");
        if (ImGui::IsItemActive()&&ImGui::IsMouseDragging()) {
            mouseScale(delta);
        }
    }
};
