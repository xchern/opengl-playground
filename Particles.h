#pragma once
#include "glshader_helper.h"

class ParticleShaderProgram {
private:
    GLuint vertex, fragment, program;
    size_t particleNumber = 0;
    GLuint vertBufPos;
    GLuint vertBufCol;
    GLuint vertBufRadius;
    GLuint vao;
public:
    ParticleShaderProgram() {
        program = glCreateProgram();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glGenBuffers(1, &vertBufPos);
        glGenBuffers(1, &vertBufCol);
        glGenBuffers(1, &vertBufRadius);
        glGenVertexArrays(1, &vao);
        setupVAO();
        compile(R"(
            #version 330
            uniform mat4 MVP;
            uniform float unitSize;
            layout (location = 0) in vec3 vPos;
            layout (location = 1) in vec3 vCol;
            layout (location = 2) in float vRadius;
            out float fDepthA;
            out float fDepthB;
            out float fDist;
            out float fBallRadius;
            out vec3 fCol;
            void main() {
                float sizeFactor = 1.0 / (1.0 + 5.0 * vRadius/gl_Position.w);
                gl_Position = MVP * vec4(vPos, 1.0);
                gl_PointSize = unitSize * vRadius * sizeFactor / gl_Position.w;
                fDist = gl_Position.w;
                fDepthA = -length(vec3(MVP[0][2], MVP[1][2], MVP[2][2]));
                fDepthB = MVP[3][2] - MVP[3][3];
                fBallRadius = vRadius * sizeFactor;
                fCol = vCol;
            }
            )", R"(
            #version 330
            in float fDepthA;
            in float fDepthB;
            in float fDist;
            in float fBallRadius;
            in vec3 fCol;
            void main() {
                vec2 pc = gl_PointCoord * 2.0 - 1.0;
                float l2 = dot(pc, pc);
                if (l2 > 1) discard;
                float dist = fDist - fBallRadius * sqrt(1.0 - l2);
                gl_FragDepth = -fDepthA + fDepthB / dist;
                gl_FragColor = vec4(fCol, 1.0);
            }
            )");
        setUnitSize(1000);
        glCheckError();
    }
    ~ParticleShaderProgram() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertBufPos);
        glDeleteBuffers(1, &vertBufCol);
        glDeleteBuffers(1, &vertBufRadius);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }
    void setData(size_t N, const float * pos, const float * col, const float * radius) {
        glBindBuffer(GL_ARRAY_BUFFER, vertBufPos);
        glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float), pos, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufCol);
        glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float), col, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufRadius);
        glBufferData(GL_ARRAY_BUFFER, N * sizeof(float), radius, GL_STREAM_DRAW);
        particleNumber = N;
        glCheckError();
    }
    void setMVP(bool transpose, const float * value) {
        int MVPLoc = glGetUniformLocation(program, "MVP");
        glUseProgram(program);
        glUniformMatrix4fv(MVPLoc, 1, transpose, value);
        glCheckError();
    }
    void setUnitSize(float unitSize) {
        int unitSizeLoc = glGetUniformLocation(program, "unitSize");
        glUseProgram(program);
        glUniform1f(unitSizeLoc, unitSize);
        glCheckError();
    }
    void draw() {
        glPointSize(16);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(program);
        glDrawArrays(GL_POINTS, 0, particleNumber);
    }
private:
    void setupVAO() {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufPos);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufCol);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufRadius);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
    }
    void compile(const char * vert_src, const char * frag_src) {
        if (!compileShader(vertex, vert_src))
            printf("vertex shader log: %s\n", getShaderInfoLog(vertex).c_str());
        if (!compileShader(fragment, frag_src))
            printf("fragment shader log: %s\n", getShaderInfoLog(fragment).c_str());
        if (!linkProgram(program, vertex, fragment))
            printf("program log: %s\n", getProgramInfoLog(program).c_str());
    }
};
