#pragma once

#include "BaseObject.h"

#include <string>
#include <stb_image.h>

class Texture : public BaseObject<Texture> {
    using Base = BaseObject<Texture>;
    friend Base;
private:
    void genObject() {
        glGenTextures(1, &objectId);
    }
    void deleteObject() {
        glDeleteTextures(1, &objectId);
    }
};

class Texture2D : public Texture {
public:
    void bind() { glBindTexture(GL_TEXTURE_2D, objectId); }
    void loadMemory(GLsizei width, GLsizei height, GLint format, const GLubyte * data) {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    void loadFile(std::string filename) {
        // load and generate the texture
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLint format;
            switch (nrChannels) {
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
            }
            loadMemory(width, height, format, data);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
};
