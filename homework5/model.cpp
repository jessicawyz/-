#define _CRT_SECURE_NO_WARNINGS
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "model.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Model::Model(const std::string& objPath, const std::string& mtlPath) {
    std::cout << "Starting to load model from: " << objPath << std::endl;
    std::cout << "MTL base path: " << mtlPath << std::endl;

    // 检查OBJ文件
    FILE* objFile = fopen(objPath.c_str(), "r");
    if (!objFile) {
        std::cout << "Error: Could not open OBJ file: " << objPath << std::endl;
        return;
    }

    // 读取OBJ文件的前几行，检查mtllib语句
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), objFile)) {
        if (strncmp(buffer, "mtllib", 6) == 0) {
            std::cout << "Found MTL reference: " << buffer;
            break;
        }
    }
    fclose(objFile);

    // 设置材质目录
    directory = mtlPath;

    if (!loadOBJ(objPath.c_str(), mtlPath.c_str())) {
        std::cout << "Failed to load OBJ file!" << std::endl;
        return;
    }

    // 打印加载的数据统计
    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    std::cout << "Loaded " << normals.size() / 3 << " normals" << std::endl;
    std::cout << "Loaded " << texCoords.size() / 2 << " texture coordinates" << std::endl;
    std::cout << "Loaded " << materials.size() << " materials" << std::endl;

    setupMesh();
}

bool Model::loadOBJ(const char* path, const char* mtlPath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // 设置加载选项
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = mtlPath;
    reader_config.triangulate = true;

    // 创建一个默认材质
    tinyobj::material_t defaultMat;
    defaultMat.name = "default";
    defaultMat.ambient[0] = defaultMat.ambient[1] = defaultMat.ambient[2] = 0.1f;
    defaultMat.diffuse[0] = defaultMat.diffuse[1] = defaultMat.diffuse[2] = 0.8f;
    defaultMat.specular[0] = defaultMat.specular[1] = defaultMat.specular[2] = 0.5f;
    defaultMat.shininess = 32.0f;

    // 加载OBJ文件
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, mtlPath);

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "Error: " << err << std::endl;
    }

    // 如果没有材质或加载失败，使用默认材质
    if (materials.empty()) {
        std::cout << "No materials found, using default material" << std::endl;
        materials.push_back(defaultMat);
    }

    // 检查每个形状使用的材质ID并修正无效的材质ID
    for (auto& shape : shapes) {
        for (auto& matId : shape.mesh.material_ids) {
            if (matId < 0 || matId >= materials.size()) {
                std::cout << "Fixing invalid material ID " << matId << " to 0" << std::endl;
                matId = 0; // 使用第一个材质（默认材质）
            }
        }
    }

    // 处理材质
    processMaterials(materials);

    // Extract vertex, normal, and texture data
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (attrib.normals.empty()) {
                normals.push_back(0.0f);
                normals.push_back(1.0f);
                normals.push_back(0.0f);
            }
            else {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }

           
            if (!attrib.texcoords.empty()) {
                size_t texcoordIndex = 2 * index.texcoord_index;
                if (texcoordIndex + 1 < attrib.texcoords.size()) {
                    float u = attrib.texcoords[texcoordIndex];
                    float v = 1.0f - attrib.texcoords[texcoordIndex + 1]; // 翻转V坐标

                    // 确保纹理坐标在[0,1]范围内
                    u = fmod(u, 1.0f);
                    v = fmod(v, 1.0f);
                    if (u < 0.0f) u += 1.0f;
                    if (v < 0.0f) v += 1.0f;

                    texCoords.push_back(u);
                    texCoords.push_back(v);
                }
                else {
                    texCoords.push_back(0.0f);
                    texCoords.push_back(0.0f);
                }
            }
        }
    }

    // 在loadOBJ函数中添加检查
    FILE* objFile = fopen(path, "r");
    if (objFile) {
        char buffer[256];
        bool foundMtllib = false;
        std::cout << "\nChecking OBJ file contents:" << std::endl;

        while (fgets(buffer, sizeof(buffer), objFile)) {
            if (strncmp(buffer, "mtllib", 6) == 0) {
                std::cout << "Found mtllib: " << buffer;
                foundMtllib = true;
            }
            if (strncmp(buffer, "usemtl", 6) == 0) {
                std::cout << "Found usemtl: " << buffer;
            }
        }

        if (!foundMtllib) {
            std::cout << "Warning: No mtllib statement found in OBJ file" << std::endl;
        }

        fclose(objFile);
    }
    // 加载纹理
    for (const auto& mat : materials) {
        std::string diffusePath = directory + "/" + mat.diffuse_texname;
        std::string specularPath = directory + "/" + mat.specular_texname;
        std::string bumpPath = directory + "/" + mat.bump_texname;

        if (!mat.diffuse_texname.empty()) {
            std::cout << "diffuse_texname No" << std::endl;
            textureIDs.push_back(loadTexture(diffusePath.c_str()));
        }
        if (!mat.specular_texname.empty()) {
            std::cout << "specular_texname No" << std::endl;
            textureIDs.push_back(loadTexture(specularPath.c_str()));
        }
        if (!mat.bump_texname.empty()) {
            std::cout << "bump_texname No" << std::endl;
            textureIDs.push_back(loadTexture(bumpPath.c_str()));
        }
    }

    return true;
}

void Model::processMaterials(const std::vector<tinyobj::material_t>& matData) {
    std::cout << "\nProcessing " << matData.size() << " materials:" << std::endl;

    for (const auto& mat : matData) {
        std::cout << "\nMaterial: [" << mat.name << "]" << std::endl;

        // 创建材质，确保所有值都有效
        Material material = {
            glm::vec3(
                mat.ambient[0] > 0 ? mat.ambient[0] : 0.1f,
                mat.ambient[1] > 0 ? mat.ambient[1] : 0.1f,
                mat.ambient[2] > 0 ? mat.ambient[2] : 0.1f
            ),
            glm::vec3(
                mat.diffuse[0] > 0 ? mat.diffuse[0] : 0.8f,
                mat.diffuse[1] > 0 ? mat.diffuse[1] : 0.8f,
                mat.diffuse[2] > 0 ? mat.diffuse[2] : 0.8f
            ),
            glm::vec3(
                mat.specular[0] > 0 ? mat.specular[0] : 0.5f,
                mat.specular[1] > 0 ? mat.specular[1] : 0.5f,
                mat.specular[2] > 0 ? mat.specular[2] : 0.5f
            ),
            mat.shininess > 0.0f ? mat.shininess : 32.0f,
            mat.diffuse_texname,
            mat.specular_texname,
            mat.bump_texname
        };

        materials.push_back(material);
    }
}


unsigned int Model::loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 设置纹理环绕和过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载纹理图片
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        std::cout << "Loading texture: " << path << std::endl;
        std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << nrChannels << std::endl;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Texture loaded successfully: " << path << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

void Model::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 计算每个顶点的大小
    const int vertexSize = 3 + 3 + 2; // position(3) + normal(3) + texcoord(2)
    std::vector<float> vertexData;
    vertexData.reserve(vertices.size() / 3 * vertexSize);

    for (size_t i = 0; i < vertices.size() / 3; i++) {
        // Position
        vertexData.push_back(vertices[i * 3 + 0]);
        vertexData.push_back(vertices[i * 3 + 1]);
        vertexData.push_back(vertices[i * 3 + 2]);

        // Normal
        if (i * 3 + 2 < normals.size()) {
            vertexData.push_back(normals[i * 3 + 0]);
            vertexData.push_back(normals[i * 3 + 1]);
            vertexData.push_back(normals[i * 3 + 2]);
        }
        else {
            vertexData.push_back(0.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(0.0f);
        }

        // Texture coordinates
        if (i * 2 + 1 < texCoords.size()) {
            vertexData.push_back(texCoords[i * 2 + 0]);
            vertexData.push_back(texCoords[i * 2 + 1]);
        }
        else {
            vertexData.push_back(0.0f);
            vertexData.push_back(0.0f);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针
    const size_t stride = (3 + 3 + 2) * sizeof(float);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    // Texture coord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}


void Model::Draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    shader.use();

    shader.setMat4("model", modelMatrix);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);

    // 设置材质属性
    if (!materials.empty()) {
        // 使用第一个材质，或者根据材质ID选择正确的材质
        const Material& mat = materials[0];
        shader.setVec3("material.ambient", mat.ambient);
        shader.setVec3("material.diffuse", mat.diffuse);
        shader.setVec3("material.specular", mat.specular);
        shader.setFloat("material.shininess", mat.shininess);

        // 设置纹理
        bool hasTexture = !textureIDs.empty();
        shader.setBool("hasTexture", hasTexture);

        if (hasTexture) {
            for (unsigned int i = 0; i < textureIDs.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

                if (i == 0) shader.setInt("material.diffuseMap", 0);
                else if (i == 1) shader.setInt("material.specularMap", 1);
            }
        }
    }

    // 绘制模型
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}






Model::~Model() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

std::string Model::convertToASCII(const std::string& path) {
    std::string result;
    for (char c : path) {
        if (static_cast<unsigned char>(c) < 128) {
            result += c;
        }
        else {
            // 替换非ASCII字符
            result += '_';
        }
    }
    return result;
}

std::string Model::getFileNameFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

// 在loadOBJ函数中添加MTL文件检查
void checkMtlFile(const char* mtlPath) {
    FILE* mtlFile = fopen(mtlPath, "r");
    if (mtlFile) {
        char buffer[256];
        std::cout << "\nChecking MTL file contents:" << std::endl;

        while (fgets(buffer, sizeof(buffer), mtlFile)) {
            if (strncmp(buffer, "newmtl", 6) == 0) {
                std::cout << "Found material definition: " << buffer;
            }
            // 打印材质属性
            if (strncmp(buffer, "Ka", 2) == 0 ||
                strncmp(buffer, "Kd", 2) == 0 ||
                strncmp(buffer, "Ks", 2) == 0) {
                std::cout << "Material property: " << buffer;
            }
        }
        fclose(mtlFile);
    }
    else {
        std::cout << "Could not open MTL file: " << mtlPath << std::endl;
    }
}