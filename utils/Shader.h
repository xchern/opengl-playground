#pragma once

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL/gl3w.h>

struct Shader {
    GLuint shader;
    Shader(GLenum type) {
        shader = glCreateShader(type);
    }
    Shader(Shader && s) {
        shader = s.shader;
        s.shader = 0;
    }
    Shader(const Shader &) = delete;
    ~Shader() { if (shader) glDeleteShader(shader); }
    Shader &operator=(const Shader &) = delete;
    void fromFile(std::string filename) {
        // reading
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            throw std::runtime_error(std::string("Shader cannot open file: ") + filename);
        }
        std::string content((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());

        // compiling
        source(content);
        try {
            compile();
        } catch (std::runtime_error & e) {
            throw std::runtime_error(
                std::string("Shader compiling file: ") + filename + "\n" + e.what()
            );
        }
    }
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
        if (!compileStatus())
            throw std::runtime_error(infoLog());
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
    Program(const Program&) = delete;
    ~Program() { if (program) glDeleteProgram(program); }
    Program &operator=(const Program &) = delete;
    void fromFiles(std::vector<std::string> files) {
        // reading & compiling files
        std::vector<Shader> shaders;
        for (const std::string &filename : files) {
            GLenum type;
            const std::string ext(filename.substr(filename.size() - 5, 5));
            if (ext == ".vert")
                type = GL_VERTEX_SHADER;
            else if (ext == ".frag")
                type = GL_FRAGMENT_SHADER;
            else throw std::runtime_error(
                std::string("Program: unknown file type: ") + filename
            );
            Shader s(type);
            s.fromFile(filename);
            shaders.push_back(std::move(s));
        }
        // linking
        link(shaders);
    }
    // simple wrappers for working with stl
    void attach(const Shader &shader) { glAttachShader(program, shader.shader); }
    void detach(const Shader &shader) { glDetachShader(program, shader.shader); }
    void link(const std::vector<Shader> & shaders) {
        for (const Shader & s : shaders) attach(s);
        link();
        for (const Shader & s : shaders) detach(s);
    }
    void link(void) {
        glLinkProgram(program);
        if (!linkStatus())
            throw std::runtime_error(std::string("Program linking:\n") + infoLog());
    }
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
        GLint r = glGetAttribLocation(program, name.c_str());
        if (r == -1) fprintf(stderr, "%s\n", (std::string() + "Program: " + name + " is not an active uniform variable").c_str());
        return r;
    }
    GLint uniformLoc(const std::string &name) {
        GLint r = glGetUniformLocation(program, name.c_str());
        if (r == -1) fprintf(stderr, "%s\n", (std::string() + "Program: " + name + " is not an active attribute variable").c_str());
        return r;
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
