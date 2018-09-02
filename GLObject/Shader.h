#pragma once

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL/gl3w.h>

#include "BaseObject.h"

class Shader : public BaseObject<Shader> {
    using Base = BaseObject<Shader>;
    friend Base;
public:
    Shader(GLenum type) : Base(glCreateShader(type)) {}
private:
    void deleteObject() {
        glDeleteShader(Base::objectId);
    }
protected:
    void source(const std::vector<std::string> & srcs) {
        std::vector<const char *> string;
        std::vector<int> length;
        for (const std::string & src : srcs) {
            string.push_back(src.c_str());
            length.push_back(src.size());
        }
        glShaderSource(Base::objectId, srcs.size(), &string[0], &length[0]);
    }
    void compile(void) {
        glCompileShader(Base::objectId);
        if (!compileStatus()) throw std::runtime_error(infoLog());
    }
    GLboolean compileStatus(void) {
        GLint isCompiled = 0;
        glGetShaderiv(Base::objectId, GL_COMPILE_STATUS, &isCompiled);
        return isCompiled;
    }
    std::string infoLog() {
        GLint maxLength = 0; // The maxLength includes the NULL character
        glGetShaderiv(Base::objectId, GL_INFO_LOG_LENGTH, &maxLength);
        std::string errorLog(maxLength, '\0');
        glGetShaderInfoLog(Base::objectId, maxLength, &maxLength, &errorLog[0]);
        return std::string(&errorLog[0]);
    }
public:
    void loadFile(std::string filename) {
        // reading
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            throw std::runtime_error(std::string("Shader cannot open file: ") + filename);
        }
        std::string content((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());

        // compiling
        try {
            loadSource({content});
        } catch (std::runtime_error & e) {
            throw std::runtime_error(
                std::string("Shader compiling files: ") + filename + "\n" + e.what()
            );
        }
    }
    void loadSource(std::vector<std::string> srcs) {
        source(srcs);
        compile();
    }
};

class Program : public BaseObject<Program> {
    using Base = BaseObject<Program>;
    friend Base;
private:
    void genObject () { Base::objectId = glCreateProgram(); }
    void deleteObject() { glDeleteProgram(Base::objectId); }
protected:
    void attach(const Shader &shader) { glAttachShader(Base::objectId, shader.getObjectId()); }
    void detach(const Shader &shader) { glDetachShader(Base::objectId, shader.getObjectId()); }
    void link(void) {
        glLinkProgram(Base::objectId);
        if (!linkStatus())
            throw std::runtime_error(std::string("Program linking:\n") + infoLog());
    }
    GLboolean linkStatus() {
        GLint isLinked = 0;
        glGetProgramiv(Base::objectId, GL_LINK_STATUS, &isLinked);
        return isLinked;
    }
    std::string infoLog(void) {
        GLint maxLength = 0;
        glGetProgramiv(Base::objectId, GL_INFO_LOG_LENGTH, &maxLength);
        std::string infoLog(maxLength, '\0');
        glGetProgramInfoLog(Base::objectId, maxLength, &maxLength, &infoLog[0]);
        return std::string(&infoLog[0]);
    }
public:
    void use() { glUseProgram(Base::objectId); }
    GLint attributeLoc(const std::string &name) {
        GLint r = glGetAttribLocation(Base::objectId, name.c_str());
        if (r == -1) fprintf(stderr, "%s\n", (std::string() + "Program: " + name + " is not an active uniform variable").c_str());
        return r;
    }
    GLint uniformLoc(const std::string &name) {
        GLint r = glGetUniformLocation(Base::objectId, name.c_str());
        if (r == -1) fprintf(stderr, "%s\n", (std::string() + "Program: " + name + " is not an active attribute variable").c_str());
        return r;
    }
    void loadFiles(std::vector<std::string> files) {
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
            s.loadFile(filename);
            shaders.push_back(std::move(s));
        }
        // linking
        link(shaders);
    }
    // simple wrappers for working with stl
    void link(const std::vector<Shader> & shaders) {
        for (const Shader & s : shaders) attach(s);
        link();
        for (const Shader & s : shaders) detach(s);
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
    void uniform(const std::string & name, glm::fmat2 value) {
        glUniformMatrix2fv(uniformLoc(name), 1, GL_FALSE, &value[0][0]);
    }
    void uniform(const std::string & name, glm::fmat3 value) {
        glUniformMatrix3fv(uniformLoc(name), 1, GL_FALSE, &value[0][0]);
    }
    void uniform(const std::string & name, glm::fmat4 value) {
        glUniformMatrix4fv(uniformLoc(name), 1, GL_FALSE, &value[0][0]);
    }
};
