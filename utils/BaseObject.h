#pragma once

#include <GL/gl3w.h>

// implement move semantics for opengl objects
// derives shader programs, buffers and textures
template <typename Derived>
class BaseObject {
protected:
    GLuint objectId;
    // construct from object id or genObject process
    BaseObject(GLuint id) : objectId(id) {}
    BaseObject() : objectId(-1) {
        static_cast<Derived *>(this)->genObject();
    }
    // destruct if not moved
    ~BaseObject() {
        if (isValid())
            static_cast<Derived *>(this)->deleteObject();
    }
    // unique object with move semantics
    BaseObject(const BaseObject &) = delete;
    BaseObject &operator=(const BaseObject &) = delete;
    BaseObject(BaseObject && t) {
        objectId = t.objectId;
        t.objectId = -1;
    }
    // to be implemented by derived class
    void genObject();
    void deleteObject();
public:
    GLuint getObjectId() const { return objectId; }
    bool isValid() { return objectId != -1; }
};
