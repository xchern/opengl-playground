#pragma once

#include <GL/gl3w.h>

class Buffer {
protected:
    GLuint buffer;
    Buffer() { glGenBuffers(1, &buffer); }
    ~Buffer() {
        if (buffer)
            glDeleteBuffers(1, &buffer);
    }
    Buffer(Buffer && b) {
        buffer = b.buffer;
        b.buffer = 0;
    }
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
};

class ArrayBuffer : public Buffer {
public:
    void bind() { glBindBuffer(GL_ARRAY_BUFFER, buffer); }
    void data(GLsizeiptr size, const GLvoid *data,
              GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }
};

class ElementArrayBuffer : public Buffer {
public:
    void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); }
    void data(GLsizeiptr size, const GLvoid *data,
              GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    }
};