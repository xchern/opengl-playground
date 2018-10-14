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
    GLint isLinked;
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
    GLint maxLength;
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

static const char vert_src[] = R"(
#version 330
layout (location = 0) in vec2 pos;
void main () {
    gl_Position = vec4(pos, 0, 1);
}
)";
static const char frag_hd[] = R"(
#version 330
uniform vec3      iResolution;
uniform float     iTime;
uniform float     iChannelTime[4];
uniform vec4      iMouse;
uniform vec4      iDate;
uniform float     iSampleRate;
uniform vec3      iChannelResolution[4];
uniform int       iFrame;
uniform float     iTimeDelta;
uniform float     iFrameRate;

// textures
/* uniform sampler2D iChannel%d; */
/* uniform samplerCube iChannel%d; */
/* uniform sampler3D iChannel%d; */

void mainImage( out vec4 c,  in vec2 f );

out vec4 outColor;
void main( void ) {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    mainImage( color, gl_FragCoord.xy );
    outColor = color;
}
)";

class ShaderToy {
private:
    char file[64] = "shader.glsl";
    GLuint vertex, fragment, program;
    GLuint vertBuf;
    GLuint vao;
public:
    ShaderToy() {
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
        loadShaderProgram();
        glCheckError();
    }
    ~ShaderToy() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertBuf);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }
    void loadShaderProgram() {
        auto shadertoy_src = readFile(file);
        if (!compileShader(vertex, vert_src))
            printf("shader log: %s\n", getShaderInfoLog(vertex).c_str());
        const char * srcs[] = {frag_hd, shadertoy_src.c_str()};
        if (!compileShader(fragment, 2, srcs))
            printf("shader log: %s\n", getShaderInfoLog(fragment).c_str());
        bool isLinked = linkProgram(program, vertex, fragment);
        if (!isLinked) printf("program log: %s\n", getProgramInfoLog(program).c_str());
    }
    void update() {
        ImGui::Begin("shader debug");

        {
            ImGui::InputText("glsl file", file, sizeof(file));
            if (ImGui::Button("load program"))
                loadShaderProgram();
        }
        ImGui::End();
    }
    void draw() {
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        {
            auto size = ImGui::GetIO().DisplaySize;
            float res[3] = {size.x, size.y, 1};
            int resLoc = glGetUniformLocation(program, "iResolution");
            glUniform3fv(resLoc, 1, res);
            int timeLoc = glGetUniformLocation(program, "iTime");
            glUniform1f(timeLoc, ImGui::GetTime());
        }
        glCheckError();
    }
};

class App : public ImGui::App {
public:
    App() : ImGui::App("shadertoy") {}
private:
    ShaderToy st;
    virtual void update() override {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        st.update();
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        st.draw();
    }
};

int main() {
    return App().exec();
}
