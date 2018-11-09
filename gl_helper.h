#pragma once

#include <string>
#include <stdio.h>
#include <assert.h>
#include <ImGuiApp.h>

// for reading shader source
inline std::string readFile(const char * filename) {
    FILE * fp = fopen(filename, "r");
    if (!fp) return "";
    // obtain file size:
    fseek (fp , 0 , SEEK_END);
    size_t size = ftell(fp);
    rewind (fp);
    // readfile
    std::string content(size + 1, '\0');
    size_t result = fread (&content[0], 1, size, fp);
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

class ProgramLoader {
public:
    static GLuint fromSource(const char * vert_src, const char * frag_src) {
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
};

// ========== buffers ==========

class BufferArray {
public:
    GLuint vao;
    int bufferNumber;
    GLuint buffers[16];
    BufferArray() : bufferNumber(0) {
        glGenVertexArrays(1, &vao);
    }
    void allocBuffers(int bufferNum) {
        glDeleteBuffers(bufferNumber, buffers);
        bufferNumber = bufferNum;
        assert(bufferNumber <= 16);
        glGenBuffers(bufferNumber, buffers);
    }
    ~BufferArray() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(bufferNumber, buffers);
    }
    void setup(int bufferNum, const int bufferDim[]) {
        allocBuffers(bufferNum);
        glBindVertexArray(vao);
        for (int i = 0; i < bufferNumber; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
            glVertexAttribPointer(i, bufferDim[i], GL_FLOAT, GL_FALSE, bufferDim[i] * sizeof(float), (void *)0);
            glEnableVertexAttribArray(i);
        }
    }
    void setData(int i, const size_t size, const float * ptr, GLenum usage = GL_DYNAMIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, usage);
    }
};
