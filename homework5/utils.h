#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>

// Draws the sliders and input controls for the light properties
void drawSlider(float x, float y, float width, float& value, float minValue, float maxValue);
void drawText(const std::string& text, float x, float y);
void drawLightEditor(struct Light& light);

// Handles mouse and input events
bool isMousePressed();
bool mouseIsOverSlider(float x, float y, float width, float height);
float map(float value, float fromMin, float fromMax, float toMin, float toMax);

#endif
