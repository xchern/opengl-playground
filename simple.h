#pragma once
#include "gl_helper.h"
#include <glm/glm.hpp>

class SimpleVertexList {
public:
    using vec3 = glm::fvec3;
    using ivec3 = glm::ivec3;
    SimpleVertexList() : col_def(0.3) { ba.setupVAO({3,3}); }
    // for opengl
    BufferArray<2> ba;
    GLuint getVAO() { return ba.vao; }
    void bufferData(GLenum usage = GL_DYNAMIC_DRAW) {
        ba.bufferData(0, pos.size() * sizeof(vec3), posPtr(), usage);
        ba.bufferData(1, col.size() * sizeof(vec3), colPtr(), usage);
    }
    // vertex list
    std::vector<vec3> pos;
    std::vector<vec3> col;
    float * posPtr() { return (float *)pos.data(); }
    float * colPtr() { return (float *)col.data(); }
    size_t vertexNumber() { return pos.size(); }
    void addVertex(vec3 v, vec3 c) { pos.push_back(v); col.push_back(c); }
    void addLine(vec3 a, vec3 b, vec3 c) { addVertex(a, c); addVertex(b, c); }
    void addAABB(vec3 a, vec3 b, vec3 c, ivec3 res = ivec3(1,1,1)) {
        for (int i = 0; i <= res.x; i++)
            for (int j = 0; j <= res.y; j++) {
                const float x = a.x + (b.x - a.x) * i / res.x;
                const float y = a.y + (b.y - a.y) * j / res.y;
                addLine(vec3(x,y,a.z), vec3(x,y,b.z), c);
            }
        for (int i = 0; i <= res.x; i++)
            for (int j = 0; j <= res.z; j++) {
                const float x = a.x + (b.x - a.x) * i / res.x;
                const float z = a.z + (b.z - a.z) * j / res.z;
                addLine(vec3(x,a.y,z), vec3(x,b.y,z), c);
            }
        for (int i = 0; i <= res.y; i++)
            for (int j = 0; j <= res.z; j++) {
                const float y = a.y + (b.y - a.y) * i / res.y;
                const float z = a.z + (b.z - a.z) * j / res.z;
                addLine(vec3(a.x,y,z), vec3(b.x,y,z), c);
            }
    }
    void addAxes(vec3 v, float l) {
        addLine(v, v + vec3(1,0,0) * l, vec3(0,0,1));
        addLine(v, v + vec3(0,1,0) * l, vec3(0,1,0));
        addLine(v, v + vec3(0,0,1) * l, vec3(1,0,0));
    }
    // default color helper
    vec3 col_def;
    void setDefaultColor(vec3 c) { col_def = c; }
    void addVertex(vec3 v) {
        pos.push_back(v); col.push_back(col_def);
    }
    void addLine(vec3 a, vec3 b) { addLine(a,b,col_def); }
    void addAABB(vec3 a, vec3 b, ivec3 res = ivec3(1,1,1)) {
        addAABB(a, b, col_def, res);
    }
};

class SimpleShaderProgram {
private:
    GLuint program;
public:
    SimpleShaderProgram() {
        static const char vertSrc[] = R"(
            #version 330
            uniform mat4 MVP;
            layout (location = 0) in vec3 vPos;
            layout (location = 1) in vec3 vCol;
            out vec3 fCol;
            void main() {
                gl_Position = MVP * vec4(vPos, 1.0);
                fCol = vCol;
            }
            )";
        static const char fragSrc[] = R"(
            #version 330
            in vec3 fCol;
            void main() {
                gl_FragColor = vec4(fCol, 1.0);
            }
            )";
        program = programFromSource(vertSrc, fragSrc);
        glCheckError();
    }
    GLuint getProgram() { return program; }
    ~SimpleShaderProgram() {
        glDeleteProgram(program);
    }
    void use() {
        glUseProgram(program);
        glCheckError();
    }
    void setMVP(bool transpose, const float * value) {
        int MVPLoc = glGetUniformLocation(program, "MVP");
        glUseProgram(program);
        glUniformMatrix4fv(MVPLoc, 1, transpose, value);
        glCheckError();
    }
};
