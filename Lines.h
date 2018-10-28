#pragma once
#include "gl_helper.h"
#include <vector>
#include <glm/glm.hpp>

class LinesShaderProgram {
private:
    GLuint program;
    size_t vertexNumber = 0;
    BufferArray ba;
public:
    LinesShaderProgram() : ba(2) {
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
        program = ProgramLoader::fromSource(vertSrc, fragSrc);
        int dim[] = {3,3};
        ba.setupVAO(dim);
        glCheckError();
    }
    ~LinesShaderProgram() {
        glDeleteProgram(program);
    }
    void setData(size_t N, const float * pos, const float * col) {
        ba.setData(0, N * 3 * sizeof(float), pos);
        ba.setData(1, N * 3 * sizeof(float), col);
        vertexNumber = N;
        glCheckError();
    }
    void setMVP(bool transpose, const float * value) {
        int MVPLoc = glGetUniformLocation(program, "MVP");
        glUseProgram(program);
        glUniformMatrix4fv(MVPLoc, 1, transpose, value);
        glCheckError();
    }
    void draw() {
        glEnable(GL_DEPTH_TEST);
        glUseProgram(program);
        glBindVertexArray(ba.vao);
        glDrawArrays(GL_LINES, 0, vertexNumber);
        glCheckError();
    }
};

class LinesBuffer {
    int lineNumber;
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    glm::vec3 col_def;
public:
    LinesBuffer() {
        lineNumber = 0;
        col_def = glm::vec3(0);
    }
    void setDefaultColor(glm::vec3 c) {
        col_def = c;
    }
    size_t vertexNumber() {
        return lineNumber * 2;
    }
    float * posPtr() {
        return (float *) pos.data();
    }
    float * colPtr() {
        return (float *) col.data();
    }
    void addLine(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        pos.push_back(a); pos.push_back(b);
        col.push_back(c); col.push_back(c);
        lineNumber++;
    }
    void addLine(glm::vec3 a, glm::vec3 b) {
        addLine(a,b,col_def);
    }
    void addAABB(glm::vec3 a, glm::vec3 b, glm::ivec3 res = glm::ivec3(1,1,1)) {
        auto c = col_def;
        col_def=glm::fvec3(0,0,1);
        for (int i = 0; i <= res.x; i++)
            for (int j = 0; j <= res.y; j++) {
                const float x = a.x + (b.x - a.x) * i / res.x;
                const float y = a.y + (b.y - a.y) * j / res.y;
                addLine(glm::vec3(x,y,a.z), glm::vec3(x,y,b.z));
            }
        col_def=glm::fvec3(0,1,0);
        for (int i = 0; i <= res.x; i++)
            for (int j = 0; j <= res.z; j++) {
                const float x = a.x + (b.x - a.x) * i / res.x;
                const float z = a.z + (b.z - a.z) * j / res.z;
                addLine(glm::vec3(x,a.y,z), glm::vec3(x,b.y,z));
            }
        col_def=glm::fvec3(1,0,0);
        for (int i = 0; i <= res.y; i++)
            for (int j = 0; j <= res.z; j++) {
                const float y = a.y + (b.y - a.y) * i / res.y;
                const float z = a.z + (b.z - a.z) * j / res.z;
                addLine(glm::vec3(a.x,y,z), glm::vec3(b.x,y,z));
            }
        col_def = c;
    }
};
