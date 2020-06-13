#pragma once

#include "BaseObject.h"

class VertexArray : BaseObject<VertexArray>{
    using Base = BaseObject<VertexArray>;
    friend Base;
private:
    void genObject() { glGenVertexArrays(1, &(Base::objectId)); }
    void deleteObject() { glDeleteVertexArrays(1, &(Base::objectId)); }
public:
    void bind() { glBindVertexArray(Base::objectId); }
};
