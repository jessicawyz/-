#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include "model.h"
#include "shader.h"

class SceneNode {
public:
    glm::mat4 transform; // Local transformation matrix
    std::vector<std::shared_ptr<Model>> models; // Models contained in this node
    std::vector<std::shared_ptr<SceneNode>> children; // Child nodes

    SceneNode() : transform(glm::mat4(1.0f)) {} // Initialize with identity matrix

    // Add a child node
    void addChild(std::shared_ptr<SceneNode> child) {
        children.push_back(child);
    }

    // Add a model to this node
    void addModel(std::shared_ptr<Model> model) {
        models.push_back(model);
    }

    // Set the transformation matrix
    void setTransform(const glm::mat4& transform) {
        this->transform = transform;
    }

    // Get the transformation matrix
    const glm::mat4& getTransform() const {
        return transform;
    }

    // Recursively draw this node and its children
    void draw(Shader& shader, const glm::mat4& parentTransform, const glm::mat4& view, const glm::mat4& projection) {
        glm::mat4 globalTransform = parentTransform * transform;

        // Draw all models in this node
        for (const auto& model : models) {
            model->Draw(shader, globalTransform, view, projection);
        }

        // Recursively draw children
        for (const auto& child : children) {
            child->draw(shader, globalTransform, view, projection);
        }
    }
};

#endif
