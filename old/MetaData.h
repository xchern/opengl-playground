#pragma once

//   TODO: get metadata from glsl code
// ========== shader program with metadata ==========
//   uniform: name, type, location
//   attribute: name, type, location
//     where type : float vec234, mat234,

class ProgramLoader {
public:
    enum type {TYPE_NONE = 0,
        TYPE_FLOAT,
        TYPE_VEC2, TYPE_VEC3, TYPE_VEC4,
        TYPE_MAT2, TYPE_MAT3, TYPE_MAT4,
    };
private:
    std::vector<std::tuple<std::string, enum type, GLint>> uniforms;
    std::vector<std::tuple<std::string, enum type, GLint>> attributes;
};
