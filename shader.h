#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>


struct Shader {
    GLuint shader;
    Shader(GLenum type) {
        shader = glCreateShader(type);
    }
    Shader(Shader && s) {
        shader = s.shader;
        s.shader = 0;
    }
    ~Shader() { if (shader) glDeleteShader(shader); }
    // simple wrappers for working with stl
    void source(const std::string & src) {
        source(std::vector<std::string>({src}));
    }
    void source(const std::vector<std::string> & srcs) {
        std::vector<const char *> string;
        std::vector<int> length;
        for (const std::string & src : srcs) {
            string.push_back(src.c_str());
            length.push_back(src.size());
        }
        glShaderSource(shader, srcs.size(), &string[0], &length[0]);
    }
    void compile(void) {
        glCompileShader(shader);
    }
    GLboolean compileStatus(void) {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        return isCompiled;
    }
    std::string infoLog() {
        GLint maxLength = 0; // The maxLength includes the NULL character
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::string errorLog(maxLength, '\0');
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        return std::string(&errorLog[0]);
    }
};

struct Program {
    GLuint program;
    Program() {
        program = glCreateProgram();
    }
    Program(Program && p) {
        program = p.program;
        p.program = 0;
    }
    ~Program() { if (program) glDeleteProgram(program); }
    // simple wrappers for working with stl
    void attach(const Shader & shader) { glAttachShader(program, shader.shader); }
    void detach(const Shader & shader) { glDetachShader(program, shader.shader); }
    void link(void) { glLinkProgram(program); }
    GLboolean linkStatus() {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        return isLinked;
    }
    std::string infoLog(void) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::string infoLog(maxLength, '\0');
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        return std::string(&infoLog[0]);
    }
    void use() { glUseProgram(program); }
    GLint attributeLoc(const std::string &name) {
        return glGetAttribLocation(program, name.c_str());
    }
    GLint uniformLoc(const std::string &name) {
        return glGetUniformLocation(program, name.c_str());
    }
    // overload with glm vector types and stl containners
    void uniform(const std::string & name, float value) {
        glUniform1fv(uniformLoc(name), 1, &value);
    }
    void uniform(const std::string & name, glm::fvec2 value) {
        glUniform2fv(uniformLoc(name), 1, &value[0]);
    }
    void uniform(const std::string & name, glm::fvec3 value) {
        glUniform3fv(uniformLoc(name), 1, &value[0]);
    }
    void uniform(const std::string & name, glm::fvec4 value) {
        glUniform4fv(uniformLoc(name), 1, &value[0]);
    }
    void uniform(const std::string & name, glm::fmat4 value) {
        glUniformMatrix4fv(uniformLoc(name), 1, GL_FALSE, &value[0][0]);
    }
};