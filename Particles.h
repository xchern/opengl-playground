#pragma once
#include "gl_helper.h"


class ParticleShaderProgram {
private:
    GLuint program;
    size_t particleNumber = 0;
    BufferArray ba;
public:
    ParticleShaderProgram() : ba(3) {
        static const char vertSrc[] = R"(
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
                gl_Position = MVP * vec4(vPos, 1.0);
                float sizeFactor = 1.0 / (1.0 + 5.0 * vRadius/gl_Position.w);
                gl_PointSize = unitSize * vRadius * sizeFactor / gl_Position.w;
                fDist = gl_Position.w;
                fDepthA = -length(vec3(MVP[0][2], MVP[1][2], MVP[2][2]));
                fDepthB = MVP[3][2] - MVP[3][3];
                fBallRadius = vRadius * sizeFactor;
                fCol = vCol;
            }
            )";
        static const char fragSrc[] = R"(
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
            )";
        program = ProgramLoader::fromSource(vertSrc, fragSrc);
        int dim[] = {3,3,1};
        ba.setupVAO(dim);
        setUnitSize(1000);
        glCheckError();
    }
    ~ParticleShaderProgram() {
        glDeleteProgram(program);
    }
    void setData(size_t N, const float * pos, const float * col, const float * radius) {
        ba.setData(0, N * 3 * sizeof(float), pos);
        ba.setData(1, N * 3 * sizeof(float), col);
        ba.setData(2, N * sizeof(float), radius);
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
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glUseProgram(program);
        glDrawArrays(GL_POINTS, 0, particleNumber);
        glCheckError();
    }
};
