#ifndef MLT_MODEL_H
#define MLT_MODEL_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class MltModel {
public:
    MltModel(const std::string& path);
    void Draw();
    ~MltModel();

private:
    std::vector<float> vertices;
    std::vector<float> normals;
    unsigned int VAO, VBO;

    void setupMesh();
    bool loadMlt(const std::string& path);
};

#endif // MLT_MODEL_H 