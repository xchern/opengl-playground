#pragma once

#include<GLFW/glfw3.h>
#ifdef USE_IMGUI
#include<imgui.h>
#endif

// Xbox 360 joystick handling
class JoyStick {
public:
    enum Button {
        BUTTON_A = 0,
        BUTTON_B,
        BUTTON_X,
        BUTTON_Y,
        BUTTON_LB,
        BUTTON_RB,
        BUTTON_START,
        BUTTON_BACK,
        BUTTON_GUIDE,
        BUTTON_LEFT_AXIS,
        BUTTON_RIGHT_AXIS,
    };
    bool getButton(enum Button code) {
        return buttons[code];
    }
    enum Axis {
        AXIS_LEFT_X=0,
        AXIS_LEFT_Y,
        AXIS_LEFT_T,
        AXIS_RIGHT_X,
        AXIS_RIGHT_Y,
        AXIS_RIGHT_T,
        AXIS_DPAD_X,
        AXIS_DPAD_Y,
    };
    float getAxis(enum Axis code) {
        const float a = axes[code];
        // discard non-zero rest value
        return a > 0.05f || a < -0.05f ? a : 0;
    }
    bool fetchState() {
        if(glfwJoystickPresent(GLFW_JOYSTICK_1)) {
            axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
            buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
            return true;
        } else
            return false;
    }
#ifdef USE_IMGUI
    void ImGuiShow() {
        ImGui::Text("%s", glfwGetJoystickName(GLFW_JOYSTICK_1));
        for (int i = 0; i < axes_count; i++) {
            if (i) ImGui::SameLine();
            ImGui::Text("%.3f", axes[i]);
        }
        for (int i = 0; i < buttons_count; i++) {
            if (i) ImGui::SameLine();
            ImGui::Text("%d", buttons[i]);
        }
    }
#endif
private:
    int buttons_count;
    const unsigned char *buttons;
    int axes_count;
    const float *axes;
};
