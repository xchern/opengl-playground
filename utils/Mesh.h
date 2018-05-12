#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <GL/gl3w.h>

#include "Buffer.h"
#include "VertexArray.h"

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

class SimpleMesh {
public:
    std::vector<glm::fvec3> vertice;
    std::vector<glm::ivec3> face;
    void readRaw(std::string filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open())
            throw std::runtime_error(std::string("TriangleMesh cannot open file: ") +
                                     filename);
        vertice.clear(); face.clear();

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
        vertice.resize(vid);
        for (const auto & p: vertmap) vertice[p.second] = p.first;
    }
    void readObj(std::string filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open())
            throw std::runtime_error(std::string("TriangleMesh cannot open file: ") +
                                     filename);
        vertice.clear(); face.clear();
        std::string line;
        while (std::getline(ifs, line)) {

            std::istringstream iss(line);
            std::string tmp;
            iss >> tmp;
            if (tmp == "v") {
                glm::fvec3 fvec3;
                iss >> fvec3.x >> fvec3.y >> fvec3.z;
                vertice.push_back(fvec3);
            }
            else if (tmp == "f") {
                glm::ivec3 ivec3;
                iss >> ivec3.x >> ivec3.y >> ivec3.z;
                ivec3.x--; ivec3.y--; ivec3.z--;
                face.push_back(ivec3);
            }
        }
    }
    void writeObj(std::string filename) {
        std::ofstream ofs(filename);
        if (!ofs.is_open())
            throw std::runtime_error(std::string("TriangleMesh cannot open file: ") +
                                     filename);
        for (const glm::fvec3 & p: vertice)
            ofs << "v " << p.x << ' ' << p.y << ' ' << p.z << '\n';
        for (const glm::ivec3 & f: vertice)
            ofs << "f " << f[0] << ' ' << f[1] << ' ' << f[2] << '\n';
        ofs << std::flush;
    }
};

class FlatTriangleMesh : protected SimpleMesh {
protected:
    std::vector<glm::fvec3> faceNorm;
    ArrayBuffer bufVertPos, bufVertNorm;
    VertexArray va;
public:
    void fromData(std::vector<glm::fvec3> vertice_, std::vector<glm::ivec3> face_) {
        vertice = vertice_;
        face = face_;
        calcFaceNorm();
    }
    void calcFaceNorm(void) {
        faceNorm.resize(face.size());
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            const glm::fvec3 a = vertice[f[0]];
            const glm::fvec3 b = vertice[f[1]];
            const glm::fvec3 c = vertice[f[2]];
            faceNorm[i] = glm::normalize(glm::cross(a - b, b - c));
        }
    }
    void copyToBuffer(GLenum usage = GL_STATIC_DRAW) {
        std::vector<glm::fvec3> buf(face.size() * 3);
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            for (int v = 0; v < 3; v++)
                buf[3*i + v] = vertice[f[v]];
        }
        bufVertPos.data(sizeof(glm::fvec3) * buf.size(), buf.data(), usage);
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            for (int v = 0; v < 3; v++)
                buf[3*i + v] = faceNorm[i];
        }
        bufVertNorm.data(sizeof(glm::fvec3) * buf.size(), buf.data(), usage);
    }
    void bindVA(GLuint posLoc, GLuint normLoc) {
        va.bind();
        bufVertPos.bind();
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(posLoc);
        bufVertNorm.bind();
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(normLoc);
    }
    void draw() {
        va.bind();
        glDrawArrays(GL_TRIANGLES, 0, face.size() * 3);
    }
};

class SmoothTriangleMesh : public FlatTriangleMesh {
protected:
    std::vector<glm::fvec3> vertNorm;
    std::vector<glm::fvec3> faceCentor;

    // opengl
    ElementArrayBuffer bufFace;
public:
    void fromData(std::vector<glm::fvec3> vertice_, std::vector<glm::ivec3> face_) {
        FlatTriangleMesh::fromData(vertice_, face_);
        calcVertexNorm();
    }
    void fromData(
            std::vector<glm::fvec3> vertice_,
            std::vector<glm::fvec3> vertNorm_,
            std::vector<glm::ivec3> face_
            ) {
        vertice = vertice_;
        vertNorm = vertNorm_;
        face = face_;
    }

    void offset(float o) {
        for (int i = 0; i < vertice.size(); i++)
            vertice[i] += vertNorm[i] * o;
    }
    void calcNorm(void) {
        calcFaceNorm();
        calcVertexNorm();
    }
    void calcVertexNorm(void) {
        vertNorm.clear();
        vertNorm.resize(vertice.size(), glm::fvec3(0, 0, 0));
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            const float a = length(vertice[f[1]] - vertice[f[2]]);
            const float b = length(vertice[f[2]] - vertice[f[0]]);
            const float c = length(vertice[f[0]] - vertice[f[1]]);
            const float A = acos((b * b + c * c - a * a) / (2.f * b * c));
            const float B = acos((a * a + c * c - b * b) / (2.f * a * c));
            const float C = acos((a * a + b * b - c * c) / (2.f * a * b));
            vertNorm[f[0]] += faceNorm[i] * A;
            vertNorm[f[1]] += faceNorm[i] * B;
            vertNorm[f[2]] += faceNorm[i] * C;
        }
        for (auto & n : vertNorm) n = glm::normalize(n);
    }

    void calcFaceCentor(void) {
        faceCentor.resize(face.size());
        for (int i = 0; i < face.size(); i++) {
            const glm::ivec3 f = face[i];
            faceCentor[i] = (vertice[f[0]] + vertice[f[1]] + vertice[f[2]]) * (1.f / 3);
        }
    }
    void copyToBuffer(GLenum usage = GL_STATIC_DRAW) {
        bufVertPos.data(sizeof(glm::fvec3) * vertice.size(), &vertice[0], usage);
        bufVertNorm.data(sizeof(glm::fvec3) * vertNorm.size(), &vertNorm[0], usage);
        bufFace.data(sizeof(glm::ivec3) * face.size(), &face[0], usage);
    }
    void bindVA(GLuint posLoc, GLuint normLoc) {
        va.bind(); bufFace.bind();
        bufVertPos.bind();
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(posLoc);
        bufVertNorm.bind();
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(normLoc);
    }
    void draw() {
        va.bind();
        glDrawElements(GL_TRIANGLES, face.size() * 3, GL_UNSIGNED_INT, (void *) 0);
    }
};

class TriangleUVMesh : public SmoothTriangleMesh {
    std::vector<glm::fvec2> vertCoord;
    ArrayBuffer bufVertCoord;
public:
    void fromData(std::vector<glm::fvec3> vertice_, std::vector<glm::ivec3> face_) {
        SmoothTriangleMesh::fromData(vertice_, face_);
        vertCoord = std::vector<glm::fvec2> (vertice.size(), {0,0}); // zero coord
    }
    void copyToBuffer(GLenum usage = GL_STATIC_DRAW) {
        SmoothTriangleMesh::copyToBuffer(usage);
        bufVertCoord.data(sizeof(glm::fvec2) * vertCoord.size(), &vertCoord[0], usage);
    }
    void bindVA(GLuint posLoc, GLuint normLoc, GLuint coordLoc) {
        SmoothTriangleMesh::bindVA(posLoc, normLoc);
        bufVertCoord.bind();
        glVertexAttribPointer(coordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(normLoc);
    }
};
