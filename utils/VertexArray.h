#pragma once

#include <GL/gl3w.h>

class VertexArray {
    GLuint va;
public:
    VertexArray() {
        glGenVertexArrays(1, &va);
    }
    ~VertexArray() {
        if (va)
            glDeleteVertexArrays(1, &va);
    }
    VertexArray(VertexArray && v) {
        va = v.va;
        v.va = 0;
    }
    VertexArray(const VertexArray &) = delete;
    VertexArray &operator=(const VertexArray &) = delete;
    void bind() { glBindVertexArray(va); }
};