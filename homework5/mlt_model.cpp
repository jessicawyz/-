#include "mlt_model.h"
#include <iostream>
#include <fstream>
#include <sstream>

MltModel::MltModel(const std::string& path) {
    if (!loadMlt(path)) {
        std::cout << "Failed to load MLT file!" << std::endl;
    }
    setupMesh();
}

bool MltModel::loadMlt(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") { // 顶点
            float x, y, z;
            if (iss >> x >> y >> z) {
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        else if (prefix == "vn") { // 法线
            float nx, ny, nz;
            if (iss >> nx >> ny >> nz) {
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }
    }

    file.close();
    return true; // 返回是否成功
}

void MltModel::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void MltModel::Draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

MltModel::~MltModel() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}