#include "ImGuiApp.h"
#include "glshader_helper.h"
#include <math.h>
#include <string>
#include <vector>

float randFloat() {
    return (float) rand() / RAND_MAX;
}

class ParticleShaderProgram {
private:
    GLuint vertex, fragment, program;
    size_t particleNumber = 0;
    GLuint vertBufPos;
    GLuint vertBufCol;
    GLuint vao;
public:
    ParticleShaderProgram() {
        program = glCreateProgram();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glGenBuffers(1, &vertBufPos);
        glGenBuffers(1, &vertBufCol);
        glGenVertexArrays(1, &vao);
        setupVAO();
        compile(R"(
            #version 330
            uniform mat4 MVP;
            layout (location = 0) in vec3 vPos;
            layout (location = 1) in vec3 vCol;
            out vec3 fCol;
            void main() {
                fCol = vCol;
                gl_Position = MVP * vec4(vPos, 1.0);
            }
            )", R"(
            #version 330
            in vec3 fCol;
            void main() {
                vec2 pc = gl_PointCoord * 2.0 - 1.0;
                if (dot(pc, pc) > 1) discard;
                gl_FragColor = vec4(fCol, 1.0);
            }
            )");
        glCheckError();
    }
    void setupVAO() {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufPos);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufCol);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }
    ~ParticleShaderProgram() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertBufPos);
        glDeleteBuffers(1, &vertBufCol);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);
    }
    void setData(size_t N, const void * pos, const void * col) {
        glBindBuffer(GL_ARRAY_BUFFER, vertBufPos);
        glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float), pos, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vertBufCol);
        glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float), col, GL_STREAM_DRAW);
        particleNumber = N;
        glCheckError();
    }
    void setMVP(bool transpose, const float * value) {
        int MVPLoc = glGetUniformLocation(program, "MVP");
        glUseProgram(program);
        glUniformMatrix4fv(MVPLoc, 1, transpose, value);
        glCheckError();
    }
    void compile(const char * vert_src, const char * frag_src) {
        if (!compileShader(vertex, vert_src))
            printf("shader log: %s\n", getShaderInfoLog(vertex).c_str());
        if (!compileShader(fragment, frag_src))
            printf("shader log: %s\n", getShaderInfoLog(fragment).c_str());
        bool isLinked = linkProgram(program, vertex, fragment);
        if (!isLinked) printf("program log: %s\n", getProgramInfoLog(program).c_str());
    }
    void draw() {
        glPointSize(8);
        glUseProgram(program);
        glDrawArrays(GL_POINTS, 0, particleNumber);
    }
};


class App : public ImGui::App {
public:
    App(int argc, char ** argv) : ImGui::App("ParticleShaderProgram") {
        ImGui::GetIO().Fonts->AddFontFromFileTTF("DejaVuSans.ttf", 18.0f);
        static const float v[4][4] = {
            {1,0,0,0},
            {0,1,0,0},
            {0,0,1,0},
            {0,0,0,1},
        };
        program.setMVP(false, (const float *)&v);
    }
private:
    ParticleShaderProgram program;
    bool p_control = true;
    bool fullscreen = false;
    char csvFile[64];
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
            ImGui::Begin("control", &p_control);
            if (ImGui::Button("load random data")) {
                const size_t N = 1000;
                std::vector<float> pos;
                std::vector<float> col;
                for (int i = 0; i < N; i++) {
                    pos.push_back(2 * randFloat() - 1);
                    pos.push_back(2 * randFloat() - 1);
                    pos.push_back(2 * randFloat() - 1);
                    col.push_back(randFloat());
                    col.push_back(randFloat());
                    col.push_back(randFloat());
                }
                program.setData(N, pos.data(), col.data());
            }
            ImGui::InputText("csv file", csvFile, sizeof(csvFile));
            if (ImGui::Button("load file")) {
                //TODO
            }
            ImGui::End();
        }
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        program.draw();
        ImDrawList * drawList = ImGui::GetOverlayDrawList();
        drawList->PushClipRectFullScreen();
        const float s = 1.2 + 0.2*cos(ImGui::GetTime()*0.3*2*M_PI);
        drawList->AddCircleFilled(ImGui::GetMousePos(), 20 * sqrt(s), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0,0.5,0.0,1*(1-exp(-1.5/s)))), 36);
        drawList->AddText(ImVec2(10,10), 0xffffffff, "nice!");
        drawList->PopClipRect();
    }
};

int main(int argc, char ** argv) {
    return App(argc, argv).exec();
}

