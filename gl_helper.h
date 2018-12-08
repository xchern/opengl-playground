#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <ImGuiApp.h>

// for reading shader source
inline std::string readFile(const char * filename) {
    FILE * fp = fopen(filename, "r");
    if (!fp) return "";
    // obtain file size:
    fseek(fp , 0 , SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    // readfile
    std::string content(size + 1, '\0');
    size_t result = fread(&content[0], 1, size, fp);
    if (result != size) {
        fputs("error reading file", stderr);
        return "";
    }
    fclose(fp);
    return content.c_str();
}

// ========== shaders ==========

inline bool compileShader(GLuint shader, int count, const char * srcs[]) {
    glShaderSource(shader, count, srcs, NULL);
    glCompileShader(shader);
    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    return isCompiled;
}

inline bool compileShader(GLuint shader, const char * src) {
    return compileShader(shader, 1, &src);
}

inline bool linkProgram(GLuint program, GLuint vert_shdr, GLuint frag_shdr) {
    glAttachShader(program, vert_shdr);
    glAttachShader(program, frag_shdr);
    glLinkProgram(program);
    glDetachShader(program, vert_shdr);
    glDetachShader(program, frag_shdr);
    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    return isLinked;
}

inline std::string getShaderInfoLog(GLuint shader) {
    GLint maxLength; // The maxLength includes the NULL character
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::string infoLog(maxLength, '\0');
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
    return &infoLog[0];
}

inline std::string getProgramInfoLog(GLuint program) {
    GLint maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::string infoLog(maxLength, '\0');
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    return infoLog.c_str();
}


// boxed shader and program, follow the interface of std::unique_ptr

class Shader {
    GLuint shader;
public:
    Shader(GLenum type) { shader = glCreateShader(type); }
    ~Shader() {glDeleteShader(shader);}
    Shader(Shader && s) {*this=std::move(s);}
    Shader & operator=(Shader && s) {
        reset(s.release());
        return *this;
    }
    Shader(const Shader &) = delete;
    Shader & operator=(const Shader & s) = delete;
    GLuint get() {return shader;}
    GLuint release() {
        GLuint s = shader;
        shader = 0;
        return s;
    }
    void reset(GLuint s) {
        glDeleteShader(shader);
        shader = s;
    }
};

class Program {
    GLuint program;
public:
    Program() { program = glCreateProgram(); }
    ~Program() {glDeleteProgram(program);}
    Program(Program && p) {*this=std::move(p);}
    Program & operator=(Program && p) {
        glDeleteProgram(program);
        program = p.program;
        p.program = 0;
        return *this;
    }
    Program(const Program &) = delete;
    Program & operator=(const Program & s) = delete;
    GLuint get() {return program;}
    GLuint release() {
        GLuint p = program;
        program = 0;
        return p;
    }
    void reset(GLuint p) {
        glDeleteProgram(program);
        program = p;
    }
};

inline GLuint programFromSource(const char * vert_src, const char * frag_src) {
    Shader vertex(GL_VERTEX_SHADER);
    if (!compileShader(vertex.get(), vert_src)) {
        printf("vertex shader log: %s\n", getShaderInfoLog(vertex.get()).c_str());
        return 0;
    }
    Shader fragment(GL_FRAGMENT_SHADER);
    if (!compileShader(fragment.get(), frag_src)) {
        printf("fragment shader log: %s\n", getShaderInfoLog(fragment.get()).c_str());
        return 0;
    }
    Program program;
    if (!linkProgram(program.get(), vertex.get(), fragment.get())) {
        printf("program log: %s\n", getProgramInfoLog(program.get()).c_str());
        return 0;
    }
    return program.release();
}

// ========== buffers ==========
// some vbos and a vao together
// setupVAO for array-of-struct memory layout

template <int NBuf>
class BufferArray {
public:
    GLuint vao;
    GLuint buffers[NBuf];
    BufferArray() {
        glGenVertexArrays(1, &vao);
        static_assert(NBuf <= 16);
        glGenBuffers(NBuf, buffers);
    }
    ~BufferArray() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(NBuf, buffers);
    }
    BufferArray(BufferArray && ba) { *this = ba; }
    BufferArray & operator=(BufferArray && ba) {
        memcpy(this, &ba, sizeof(BufferArray));
        memset(&ba, 0, sizeof(BufferArray));
    }
    BufferArray(const BufferArray &) = delete;
    BufferArray & operator=(const BufferArray &) = delete;
    void setupVAO(std::vector<int> dim) {
        assert(dim.size() == NBuf);
        setupVAO(dim.data());
    }
    void setupVAO(const int bufferDim[]) {
        glBindVertexArray(vao);
        for (int i = 0; i < NBuf; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
            glVertexAttribPointer(i, bufferDim[i], GL_FLOAT, GL_FALSE, bufferDim[i] * sizeof(float), (void *)0);
            glEnableVertexAttribArray(i);
        }
    }
    void bufferData(int i, const size_t size, const float * ptr, GLenum usage = GL_DYNAMIC_DRAW) {
        assert(i < NBuf);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, usage);
    }
};

//   TODO: get metadata
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
