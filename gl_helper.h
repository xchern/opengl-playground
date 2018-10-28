#pragma once

#include <string>
#include <stdio.h>
#include <assert.h>
#include <ImGuiApp.h>

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
    std::string errorLog(maxLength, '\0');
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    return &errorLog[0];
}

inline std::string getProgramInfoLog(GLuint program) {
    GLint maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::string infoLog(maxLength, '\0');
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    return infoLog.c_str();
}


class ProgramLoader {
    GLuint vertex, fragment, program;
    ProgramLoader() {
        program = glCreateProgram();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
    }
    ~ProgramLoader() {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }
    bool compile(const char * vert_src, const char * frag_src) {
        if (!compileShader(vertex, vert_src)) {
            printf("vertex shader log: %s\n", getShaderInfoLog(vertex).c_str());
            return false;
        }
        if (!compileShader(fragment, frag_src)) {
            printf("fragment shader log: %s\n", getShaderInfoLog(fragment).c_str());
            return false;
        }
        if (!linkProgram(program, vertex, fragment)) {
            printf("program log: %s\n", getProgramInfoLog(program).c_str());
            return false;
        }
        return true;
    }
    GLuint giveProgram() {
        GLuint p = program;
        program = 0;
        return p;
    }
public:
    static GLuint fromSource(const char * vert_src, const char * frag_src) {
        ProgramLoader pl;
        if (pl.compile(vert_src, frag_src))
            return pl.giveProgram();
        else
            return 0;
    }
};

// ========== buffers ==========

class BufferArray {
public:
    GLuint vao;
    int buffersNumber;
    GLuint buffers[16];
    BufferArray(int bufferNum) : buffersNumber(bufferNum) {
        assert(buffersNumber <= 16);
        glGenVertexArrays(1, &vao);
        glGenBuffers(buffersNumber, buffers);
    }
    ~BufferArray() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(buffersNumber, buffers);
    }
    void setupVAO(const int dim[]) {
        glBindVertexArray(vao);
        for (int i = 0; i < buffersNumber; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
            glVertexAttribPointer(i, dim[i], GL_FLOAT, GL_FALSE, dim[i] * sizeof(float), (void *)0);
            glEnableVertexAttribArray(i);
        }
    }
    void setData(int i, const size_t size, const float * ptr, GLenum usage = GL_DYNAMIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, usage);
    }
};
