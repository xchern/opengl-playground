#pragma once

#include "BaseObject.h"

template <GLenum target>
class Buffer : public BaseObject<Buffer<target>> {
    using Base = BaseObject<Buffer>;
    friend Base;
private:
    void genObject() {
        glGenBuffers(1, &(Base::objectId));
    }
    void deleteObject() {
        glDeleteBuffers(1, &(Base::objectId));
    }
public:
    void bind() { glBindBuffer(target, Base::objectId); }
    void data(GLsizeiptr size, const GLvoid *data,
              GLenum usage = GL_STATIC_DRAW) {
        bind();
        glBufferData(target, size, data, usage);
    }
};

class ArrayBuffer : public Buffer<GL_ARRAY_BUFFER> {};

class ElementArrayBuffer : public Buffer<GL_ELEMENT_ARRAY_BUFFER> {};
