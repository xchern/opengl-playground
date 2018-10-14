#pragma once

#include <string>
#include <stdio.h>

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
