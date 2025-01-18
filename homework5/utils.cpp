#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "utils.h"

// Define the Light struct (make sure this is done before using it)
struct Light {
    glm::vec3 position;
    float intensity;
    glm::vec3 color;
};

double lastX, lastY;

bool isMousePressed() {
    return glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

bool mouseIsOverSlider(float x, float y, float width, float height) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);

    return (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
}

float map(float value, float fromMin, float fromMax, float toMin, float toMax) {
    return (value - fromMin) / (fromMax - fromMin) * (toMax - toMin) + toMin;
}

void drawSlider(float x, float y, float width, float& value, float minValue, float maxValue) {
    // Draw the slider background (rectangle)
    glColor3f(0.5f, 0.5f, 0.5f); // Grey background
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + 10);
    glVertex2f(x, y + 10);
    glEnd();

    // Draw the slider thumb
    float thumbPosX = x + (value - minValue) / (maxValue - minValue) * width;
    glColor3f(0.8f, 0.2f, 0.2f); // Red thumb
    glBegin(GL_QUADS);
    glVertex2f(thumbPosX - 5, y - 5);
    glVertex2f(thumbPosX + 5, y - 5);
    glVertex2f(thumbPosX + 5, y + 15);
    glVertex2f(thumbPosX - 5, y + 15);
    glEnd();

    // Check for mouse drag and update value
    if (isMousePressed() && mouseIsOverSlider(x, y, width, 10)) {
        double mouseX, mouseY;
        glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
        value = map(mouseX, x, x + width, minValue, maxValue);
    }
}

void drawText(const std::string& text, float x, float y) {
    // You can use any text rendering library to render text, e.g., FreeType
    // Here we will just print the text to the console as an example
    std::cout << text << " at (" << x << ", " << y << ")\n";
}

void drawLightEditor(Light& light) {
    drawSlider(100, 100, 200, light.position.x, -10.0f, 10.0f); // x-position
    drawSlider(100, 130, 200, light.position.y, -10.0f, 10.0f); // y-position
    drawSlider(100, 160, 200, light.position.z, -10.0f, 10.0f); // z-position

    drawSlider(100, 200, 200, light.intensity, 0.0f, 10.0f); // Intensity
    drawSlider(100, 250, 200, light.color.r, 0.0f, 1.0f); // Red channel
    drawSlider(100, 280, 200, light.color.g, 0.0f, 1.0f); // Green channel
    drawSlider(100, 310, 200, light.color.b, 0.0f, 1.0f); // Blue channel
}
