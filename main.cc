#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define glCheckError                                                                    \
    do {                                                                                \
        GLenum err = glGetError();                                                      \
        if (err != GL_NO_ERROR)                                                         \
            fprintf(stderr, "OpenGL error in file '%s' in line %i, Error Code 0x%x.\n", \
                    __FILE__, __LINE__, err);                                           \
    } while (0)

static fvec2 points[] = {
    {.5, .5}, {-.5, .5}, {-.5, -.5},
    {-.5, -.5}, {.5, -.5}, {.5, .5},
};

static const std::string vertex_src = {
#include "shader.vert.inc"
};

static const std::string fragment_src = {
#include "shader.frag.inc"
};

void checkShader(GLuint shader) {
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        cout << &errorLog[0] << endl;
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(shader); // Don't leak the shader.

        exit(1);
    }
}

void checkProgram(GLuint program) {
    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        //Use the infoLog as you see fit.
        cout << &infoLog[0] << endl;

        glDeleteProgram(program);
        //In this simple program, we'll just leave
        exit(1);
    }
}

int main () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, 0);

    GLFWwindow * window = glfwCreateWindow(800, 600, "demo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    // shader
    const char * src; int len;

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    src = vertex_src.c_str(); len = vertex_src.size();
    glShaderSource(vertex, 1, &src, &len);
    glCompileShader(vertex);
    checkShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    src = fragment_src.c_str(); len = fragment_src.size();
    glShaderSource(fragment, 1, &src, &len);
    glShaderSource(fragment, 1, &src, &len);
    glCompileShader(fragment);
    checkShader(fragment);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkProgram(program);

    glDetachShader(program, vertex);
    glDeleteShader(vertex);
    glDetachShader(program, fragment);
    glDeleteShader(fragment);

    // vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // buffer
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);


    glUseProgram(program);
    GLuint pos_loc = glGetAttribLocation(program, "vPos");
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);
    glEnableVertexAttribArray(pos_loc);
    glCheckError;

    glClearColor(0, .3, .3, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLint trans_loc = glGetUniformLocation(program, "translation");
    while (!glfwWindowShouldClose(window)) {
        {
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glViewport(0, 0, w, h);
        }
        {
            static float t = 0; t += .01;
            float x = .5 * cosf(t);
            float y = .5 * sinf(t);
            glUniform2f(trans_loc, x, y);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(points) / sizeof(fvec2));
        glCheckError;
        //glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}
