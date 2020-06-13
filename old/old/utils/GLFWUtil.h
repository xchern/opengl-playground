#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cassert>

#include <map>

class GLFWInit {
public:
    GLFWInit() { glfwInit(); }
    ~GLFWInit() { glfwTerminate(); }
};


class GLFWApp {
private:
    //virtual 
    static std::map<GLFWwindow *, GLFWApp *> windowOwnner;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        windowOwnner[window]->key_callback(key, scancode, action, mods);
    }
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        windowOwnner[window]->cursor_position_callback(xpos, ypos);
    }
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        windowOwnner[window]->mouse_button_callback(button, action, mods);
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        windowOwnner[window]->scroll_callback(xoffset, yoffset);
    }
    static void drop_callback(GLFWwindow* window, int count, const char** paths) {
        windowOwnner[window]->drop_callback(count, paths);
    }
    void setupCallback() {
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetDropCallback(window, drop_callback);
    }
protected:
    GLFWwindow * window;
    virtual void draw() {}
    virtual void key_callback(int key, int scancode, int action, int mods) {}
    virtual void cursor_position_callback(double xpos, double ypos) {}
    virtual void mouse_button_callback(int button, int action, int mods) {}
    virtual void scroll_callback(double xoffset, double yoffset) {}
    virtual void drop_callback(int count, const char** paths) {}
public:
    GLFWApp() : GLFWApp("App", 960, 720) {}
    GLFWApp(const char * title, int width, int height) {
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        assert(window);
        windowOwnner[window] = this;
        setupCallback();
        glfwMakeContextCurrent(window);
        //glfwSwapInterval(0);
        gl3wInit();
    }
    ~GLFWApp() {
        windowOwnner.erase(windowOwnner.find(window));
        glfwDestroyWindow(window);
    }
    bool loopOnce() {
        if (glfwWindowShouldClose(window)) return false;
        glfwMakeContextCurrent(window);
        draw();
        glfwPollEvents();
        return true;
    }
};
std::map<GLFWwindow *, GLFWApp *> GLFWApp::windowOwnner;
