#include "ImGuiApp.h"
#include "glshader_helper.h"
#include <string>

static const char vert_src[] = R"(
#version 330
layout (location = 0) in vec2 pos;
void main () {
    gl_Position = vec4(pos, 0.0, 1.0);
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

// TODO: textures 
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
    char file[64] = "";
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

        if (!compileShader(vertex, vert_src))
            printf("shader log: %s\n", getShaderInfoLog(vertex).c_str());
        loadShaderToy("void mainImage(out vec4 c, in vec2 f) {c=vec4(f/iResolution.xy,1,1);}");
    }
    ~ShaderToy() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertBuf);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }

    void setFile(const char * filename) {
        strncpy(file, filename, 64);
        loadShaderToy(readFile(file).c_str());
        glCheckError();
    }

    void loadShaderToy(const char * shadertoy_src) {
        const char * srcs[] = {frag_hd, shadertoy_src};
        if (!compileShader(fragment, 2, srcs))
            printf("shader log: %s\n", getShaderInfoLog(fragment).c_str());
        bool isLinked = linkProgram(program, vertex, fragment);
        if (!isLinked) printf("program log: %s\n", getProgramInfoLog(program).c_str());
    }
    void update() {
        ImGui::InputText("glsl file", file, sizeof(file));
        if (ImGui::Button("load program"))
            loadShaderToy(readFile(file).c_str());
    }
    void draw() {
        glUseProgram(program);
        {
            const auto & io = ImGui::GetIO();
            const auto size = io.DisplaySize;
            const float res[3] = {size.x, size.y, 1};
            const int resLoc = glGetUniformLocation(program, "iResolution");
            glUniform3fv(resLoc, 1, res);
            const int timeLoc = glGetUniformLocation(program, "iTime");
            glUniform1f(timeLoc, ImGui::GetTime());
            const int mouseLoc = glGetUniformLocation(program, "iMouse");
            const auto p = io.MouseClickedPos[0];
            const float clicked = ImGui::IsMouseDown(0) ? 1 : -1;
            glUniform4f(mouseLoc,
                    io.MousePos.x, size.y - io.MousePos.y,
                    clicked * p.x, clicked * (size.y - p.y));
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

class App : public ImGui::App {
public:
    App(int argc, char ** argv) : ImGui::App("shadertoy", 800, 600) {
        if (argc == 2) st.setFile(argv[1]);
    }
private:
    ShaderToy st;
    bool p_control = false;
    bool fullscreen = false;
    virtual void update() override {
        const float dist = 10.f;
        ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - dist, dist);
        ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
        if (ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
            ImGui::Text("framerate: %.1ffps", ImGui::GetIO().Framerate);
            ImGui::Checkbox("show control window", &p_control);
            if (ImGui::Checkbox("fullscreen", &fullscreen))
                setFullScreen(fullscreen);
        }
        ImGui::End();
        if (p_control) {
            ImGui::Begin("shader control", &p_control);
            st.update();
            ImGui::End();
        }
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        st.draw();
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}
