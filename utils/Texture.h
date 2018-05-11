#pragma once

#include <string>

#include <GL/gl3w.h>

class Texture {
protected:
    GLuint texture;
    ~Texture() {
        glDeleteTextures(1, &texture);
    }
public:
    Texture() {
        glGenTextures(1, &texture);
    }
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture && t) {
        texture = t.texture;
        t.texture = 0;
    }
};

class Texture2D : public Texture {
public:
    void bind() { glBindTexture(GL_TEXTURE_2D, texture); }
    void image(GLsizei width, GLsizei height, GLint format, const GLubyte * data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
    }
};