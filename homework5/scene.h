#ifndef SCENE_H
#define SCENE_H

#include "scene_node.h"

class Scene {
public:
    std::shared_ptr<SceneNode> root; // Root node of the scene

    Scene() : root(std::make_shared<SceneNode>()) {}

    // Add a node to the scene graph
    void addNode(std::shared_ptr<SceneNode> node, std::shared_ptr<SceneNode> parent = nullptr) {
        if (!parent) {
            parent = root; // Default to the root node
        }
        parent->addChild(node);
    }

    // Draw the entire scene
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        root->draw(shader, glm::mat4(1.0f), view, projection);
    }

};

#endif
#pragma once