#include "ImGuiApp.h"
#include <string>

bool compileShader(GLuint shader, int count, const char * srcs[]) {
    glShaderSource(shader, count, srcs, NULL);
    glCompileShader(shader);
    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    return isCompiled;
}

bool compileShader(GLuint shader, const char * src) {
    return compileShader(shader, 1, &src);
}

bool linkProgram(GLuint program, GLuint vert_shdr, GLuint frag_shdr) {
    
    glAttachShader(program, vert_shdr);
    glAttachShader(program, frag_shdr);
    glLinkProgram(program);
    glDetachShader(program, vert_shdr);
    glDetachShader(program, frag_shdr);
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    return isLinked;
}

std::string getShaderInfoLog(GLuint shader) {
    GLint maxLength; // The maxLength includes the NULL character
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::string errorLog(maxLength, '\0');
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    return &errorLog[0];
}

std::string getProgramInfoLog(GLuint program) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::string infoLog(maxLength, '\0');
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    return infoLog.c_str();
}

std::string readFile(const char * filename) {
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

class App : public ImGui::App {
private:
    char vert_file[64] = "shader.vert";
    char frag_file[64] = "shader.frag";
    GLuint vertex, fragment, program;
    GLuint vertBuf;
    GLuint vao;
public:
    App() : ImGui::App("shadertoy") {
        program = glCreateProgram();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

        glGenBuffers(1, &vertBuf);
        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        static const float pos[] = {
            -1,-1, 1,-1, -1, 1,
             1,-1, 1, 1, -1, 1
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glCheckError();
    }
    ~App() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertBuf);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }
    void loadShaderProgram() {
        auto vert_src = readFile(vert_file);
        auto frag_src = readFile(frag_file);
        if (!compileShader(vertex, vert_src.c_str()))
            printf("shader log: %s\n", getShaderInfoLog(vertex).c_str());
        if (!compileShader(fragment, frag_src.c_str()))
            printf("shader log: %s\n", getShaderInfoLog(fragment).c_str());
        bool isLinked = linkProgram(program, vertex, fragment);
        if (!isLinked) printf("program log: %s\n", getProgramInfoLog(program).c_str());
    }
    virtual void update() override {
        /* glClearColor(0.8, 0.8, 0.8, 1.0); */
        /* glClear(GL_COLOR_BUFFER_BIT); */

        ImGui::Begin("shader debug");

        {
            ImGui::InputText("vertex file", vert_file, sizeof(vert_file));
            ImGui::InputText("fragment file", frag_file, sizeof(frag_file));
            if (ImGui::Button("load program"))
                loadShaderProgram();
        }
        if (ImGui::Button("use program"))
            glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ImGui::End();
        //glCheckError();
    }
};

int main() {
    return App().exec();
}
