#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <GL/gl3w.h>

namespace glm {
inline bool operator<(fvec3 a, fvec3 b) { // for use in map
    if (a.x < b.x) return true;
    if (a.x > b.x) return false;
    if (a.y < b.y) return true;
    if (a.y > b.y) return false;
    if (a.z < b.z) return true;
    if (a.z > b.z) return false;
    return false;
};
}

class TriangleMesh { public:
    std::vector<glm::fvec3> vert;
    std::vector<glm::fvec3> norm;

    std::vector<glm::ivec3> face;
    std::vector<glm::fvec3> faceNorm;
    std::vector<glm::fvec3> faceCentor;

    // opengl
    GLuint bufVert, bufNorm, bufFace, vao;

    TriangleMesh() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &bufVert);
        glGenBuffers(1, &bufNorm);
        glGenBuffers(1, &bufFace);
    }
    ~TriangleMesh() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &bufVert);
        glDeleteBuffers(1, &bufNorm);
        glDeleteBuffers(1, &bufFace);
    }
    void copyToBuffer(GLenum usage = GL_STATIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, bufVert);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::fvec3) * vert.size(), &vert[0], usage);

        glBindBuffer(GL_ARRAY_BUFFER, bufNorm);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::fvec3) * norm.size(), &norm[0], usage);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufFace);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * face.size(), &face[0], usage);
    }
    void bind(GLuint posLoc, GLuint normLoc) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, bufVert);
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(posLoc);
        glBindBuffer(GL_ARRAY_BUFFER, bufNorm);
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(normLoc);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufFace);
    }
    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, face.size() * 3, GL_UNSIGNED_INT, (void *) 0);
    }

    void offset(float o) {
        for (int i = 0; i < vert.size(); i++)
            vert[i] += norm[i] * o;
    }
    void calcNorm(void) {
        calcFaceNorm();
        calcVertexNorm();
    }
    void calcVertexNorm(void) {
        norm.clear();
        norm.resize(vert.size(), glm::fvec3(0, 0, 0));
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            norm[f[0]] += faceNorm[i];
            norm[f[1]] += faceNorm[i];
            norm[f[2]] += faceNorm[i];
        }
        for (auto & n : norm) n = glm::normalize(n);
    }
    void calcFaceCentor(void) {
        faceCentor.resize(face.size());
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            faceCentor[i] = (vert[f[0]] + vert[f[1]] + vert[f[2]]) * (1.f / 3);
        }
    }
    void calcFaceNorm(void) {
        faceNorm.resize(face.size());
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            const glm::fvec3 a = vert[f[0]];
            const glm::fvec3 b = vert[f[1]];
            const glm::fvec3 c = vert[f[2]];
            faceNorm[i] = glm::normalize(glm::cross(a - b, b - c));
        }
    }
    bool readRaw(std::string filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) return false;
        vert.clear(); face.clear();

        glm::fvec3 a, b, c; int vid = 0;
        glm::ivec3 f;
        std::map<glm::fvec3, int> vertmap;

        std::string line;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);
            std::string tmp;
            if(iss >> a.x >> a.y >> a.z
                >> b.x >> b.y >> b.z
                >> c.x >> c.y >> c.z ){
                if (!vertmap.count(a)) vertmap[a] = vid++;
                if (!vertmap.count(b)) vertmap[b] = vid++;
                if (!vertmap.count(c)) vertmap[c] = vid++;
                f = {vertmap[a], vertmap[b], vertmap[c]};
                face.push_back(f);
            }
        }
        vert.resize(vid);
        for (const auto & p: vertmap) vert[p.second] = p.first;
        return true;
    }
    bool readObj(std::string filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) return false;
        vert.clear(); face.clear();
        std::string line;
        while (std::getline(ifs, line)) {

            std::istringstream iss(line);
            std::string tmp;
            iss >> tmp;
            if (tmp == "v") {
                glm::fvec3 fvec3;
                iss >> fvec3.x >> fvec3.y >> fvec3.z;
                vert.push_back(fvec3);
            }
            else if (tmp == "f") {
                glm::ivec3 ivec3;
                iss >> ivec3.x >> ivec3.y >> ivec3.z;
                ivec3.x--; ivec3.y--; ivec3.z--;
                face.push_back(ivec3);
            }
        }
        return true;
    }
    bool writeObj(std::string filename) {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) return false;
        for (const glm::fvec3 & p: vert)
            ofs << "v " << p.x << ' ' << p.y << ' ' << p.z << '\n';
        for (const glm::ivec3 & f: vert)
            ofs << "f " << f[0] << ' ' << f[1] << ' ' << f[2] << '\n';
        ofs << std::flush;
        return true;
    }
};