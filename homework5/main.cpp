#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "scene.h"
#include "shader.h"
#include "model.h"
#include <iostream>

// Callback function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
void processInput(GLFWwindow* window, std::shared_ptr<SceneNode> robotNode);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

// Screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraTarget; // 相机环绕的中心点

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Orbit
float orbitRadius = 1.0f; // 轨道半径
float orbitAngle = 0.0f;  // 轨道角度
bool isOrbiting = false; // 是否正在环绕

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool cursorDisabled = true;

bool isRobotView = false; // Current view mode
bool yKeyPressed = false; // Whether Y key is pressed
glm::vec3 robotPosition = glm::vec3(1.0f, 0.0f, 2.0f); // Robot's position

// Lighting properties
glm::vec3 lightPos(2.0f, 5.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
float lightIntensity = 1.0f; // Light intensity

// For translation
float translationSpeed = 0.005f;  // Speed of translation
bool isRotating = false;    // Tracks whether the robot is rotating
float rotationAngle = 0.0f; // Cumulative rotation angle
float rotationSpeed = 50.0f; // Rotation speed in degrees per second
float zoomSpeed = 1.0f; // Speed of zoom
float zoomFactor = 1.0f; // Initial zoom scale
float fov = 45.0f; // Default field of view
bool isCameraLockedToRobot = false;

// 定义机器人的移动边界
const float robotMinY = 0.0f; // 最小Y值
const float robotMaxY = 3.0f;  // 最大Y值
const float robotMinX = -2.7f; // 最左X值
const float robotMaxX = 2.0f;  // 最右X值

// Robot transformation
glm::mat4 robotModelTransform = glm::mat4(1.0f);

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Controls with GLFW", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPos(window, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Load your shaders here
    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    // Create scene nodes and models (not implemented here)
    Scene scene;
    auto roomNode = std::make_shared<SceneNode>();
    auto roomModel = std::make_shared<Model>("..\\Model\\room_escape_new_warm_white.obj", "..\\Model");
    auto roomModel_1 = std::make_shared<Model>("..\\Model\\room_escape_new_light_blue.obj", "..\\Model");
    auto roomModel_2 = std::make_shared<Model>("..\\Model\\room_escape_new_wheel.obj", "..\\Model");
    auto roomModel_3 = std::make_shared<Model>("..\\Model\\room_escape_new_green.obj", "..\\Model");
    auto roomModel_4 = std::make_shared<Model>("..\\Model\\room_escape_new_heavy_blue.obj", "..\\Model");
    auto roomModel_5 = std::make_shared<Model>("..\\Model\\room_escape_new_wooden.obj", "..\\Model");
    auto roomModel_6 = std::make_shared<Model>("..\\Model\\room_escape_new_grey.obj", "..\\Model");
    auto roomModel_7 = std::make_shared<Model>("..\\Model\\room_escape_new_yellow.obj", "..\\Model");
    auto roomModel_8 = std::make_shared<Model>("..\\Model\\room_escape_new_mihuang.obj", "..\\Model");
    auto roomModel_9 = std::make_shared<Model>("..\\Model\\room_escape_new_pink.obj", "..\\Model");
    auto roomModel_10 = std::make_shared<Model>("..\\Model\\room_escape_new_zise.obj", "..\\Model");
    auto roomModel_11 = std::make_shared<Model>("..\\Model\\room_escape_new_red.obj", "..\\Model");
    auto roomModel_12 = std::make_shared<Model>("..\\Model\\room_escape_new_orange.obj", "..\\Model");
    auto roomModel_13 = std::make_shared<Model>("..\\Model\\room_escape_new_cafe.obj", "..\\Model");
    auto roomModel_14 = std::make_shared<Model>("..\\Model\\room_escape_new_jinshu.obj", "..\\Model");
    auto roomModel_15 = std::make_shared<Model>("..\\Model\\room_escape_new_yitui.obj", "..\\Model");
    auto roomModel_17 = std::make_shared<Model>("..\\Model\\room_escape_new_waichuangkuang.obj", "..\\Model");
    auto roomModel_18 = std::make_shared<Model>("..\\Model\\room_escape_new_clock_panel.obj", "..\\Model");
    auto roomModel_19 = std::make_shared<Model>("..\\Model\\room_escape_new_xiaoguizi.obj", "..\\Model");
    auto roomModel_20 = std::make_shared<Model>("..\\Model\\room_escape_new_floor.obj", "..\\Model");
    auto roomModel_21 = std::make_shared<Model>("..\\Model\\room_escape_new_menkuang.obj", "..\\Model");
    




    

    roomNode->addModel(roomModel);
    roomNode->addModel(roomModel_1);
    roomNode->addModel(roomModel_2);
    roomNode->addModel(roomModel_3);
    roomNode->addModel(roomModel_4);
    roomNode->addModel(roomModel_5);
    roomNode->addModel(roomModel_6);
    roomNode->addModel(roomModel_7);
    roomNode->addModel(roomModel_8);
    roomNode->addModel(roomModel_9);
    roomNode->addModel(roomModel_10);
    roomNode->addModel(roomModel_11); 
    roomNode->addModel(roomModel_12);
    roomNode->addModel(roomModel_13);
    roomNode->addModel(roomModel_14);
    roomNode->addModel(roomModel_15);
    roomNode->addModel(roomModel_17);
    roomNode->addModel(roomModel_18);
    roomNode->addModel(roomModel_19);
    roomNode->addModel(roomModel_20);
    roomNode->addModel(roomModel_21);







    scene.addNode(roomNode);

    auto robotNode = std::make_shared<SceneNode>();
    auto robotModel = std::make_shared<Model>("..\\Model\\Robo8.obj", "..\\Model");
    //auto robotModel = std::make_shared<Model>("D:\\Cusor\\Robo8.obj", "D:\Cusor");

    robotNode->addModel(robotModel);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 2.0f));
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    robotNode->transform = translation * scaling;
    scene.addNode(robotNode, roomNode);


    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //processInput(window);
        processInput(window, robotNode);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Set lighting properties in the shader
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.color", lightColor);
        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.045f);
        shader.setFloat("light.quadratic", 0.0075f);
        shader.setVec3("viewPos", cameraPos);
        shader.setFloat("lightIntensity", lightIntensity);

        // Set ambient light strength
        shader.setFloat("ambientStrength", 0.2f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        // Reset transformation for the robot
        glm::mat4 baseTransform = glm::translate(glm::mat4(1.0f), robotPosition) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

        // Apply rotation if enabled
        if (isRotating) {
            rotationAngle += rotationSpeed * deltaTime; // Adjust rotation speed (degrees per second)
        }
        else {
            // Stop accumulating rotation when not rotating
            rotationAngle = rotationAngle; // No change
        }
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        // Apply scaling based on zoom factor
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(zoomFactor, zoomFactor, zoomFactor));

        // Combine transformations
        robotModelTransform = baseTransform * rotation * scaling;


        // Update the robot node's transform
        robotNode->transform = robotModelTransform;

        // Draw the scene
        scene.draw(shader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool lightIntensityIncreased = false; // To track if intensity was already increased
bool lightIntensityDecreased = false; // To track if intensity was already decreased
bool isLightPositionMappingEnabled = false; // To track L for light position
bool qKeyPressed = false; // Tracks whether the Q key is currently pressed

void processInput(GLFWwindow* window, std::shared_ptr<SceneNode> robotNode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (cursorDisabled) {
        float cameraSpeed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        // 轨道运动
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            isOrbiting = true;
            orbitAngle -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            isOrbiting = true;
            orbitAngle += cameraSpeed;
        }
        if (!glfwGetKey(window, GLFW_KEY_O) && !glfwGetKey(window, GLFW_KEY_P)) {
            isOrbiting = false;
        }

        // 更新相机位置（轨道运动）
        if (isOrbiting) {
            cameraPos = cameraTarget + glm::vec3(
                orbitRadius * cos(glm::radians(5 * orbitAngle)),
                1.0f, // 保持相机高度不变
                orbitRadius * sin(glm::radians(5 * orbitAngle))
            );
            cameraFront = glm::normalize(cameraPos - cameraTarget);
        }

        // 切换视角
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !yKeyPressed) {
            yKeyPressed = true;
            if (isRobotView) {
                // 切换到环境视角
                cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
                cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
            }
            else {
                // 切换到机器人视角
                glm::vec3 robotHeadPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f);
                glm::vec3 robotFrontDir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
                cameraPos = robotHeadPos;
                cameraFront = robotFrontDir;

            }
            isRobotView = !isRobotView;
        }
        else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
            yKeyPressed = false;
        }

        // Adjust light intensity with '+' and '-'
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && !lightIntensityIncreased) {
            if (lightIntensity < 10.0f) {
                lightIntensity += 0.1f;
                std::cout << "Light intensity increased to: " << lightIntensity << std::endl;
            }
            lightIntensityIncreased = true;
        }
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_RELEASE) {
            lightIntensityIncreased = false;
        }

        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && !lightIntensityDecreased) {
            if (lightIntensity > 0.1f) {
                lightIntensity -= 0.1f;
                std::cout << "Light intensity decreased to: " << lightIntensity << std::endl;
            }
            lightIntensityDecreased = true;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_RELEASE) {
            lightIntensityDecreased = false;
        }


        // 切换视角
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !yKeyPressed) {
            yKeyPressed = true;
            glm::vec3 robotCurrentPos = glm::vec3(robotNode->transform[3]); // 获取机器人当前位置
            std::cout << "Robot position: (" << robotCurrentPos.x << ", " << robotCurrentPos.y << ", " << robotCurrentPos.z << ")" << std::endl;
            cameraPos = robotCurrentPos + glm::vec3(0.0f, 5.0f, 5.0f); // 假设相机在机器人上方一定高度
            cameraFront = glm::normalize(cameraPos -robotCurrentPos ); // 朝向机器人
            // 锁定摄像机位置，但允许视角旋转
            isCameraLockedToRobot = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
            yKeyPressed = false;
            isCameraLockedToRobot = false;
        }

        // 更新摄像机方向（基于鼠标输入）
        if (cursorDisabled && !isCameraLockedToRobot) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos); // 使用 double 类型变量接收光标位置

            float xoffset = static_cast<float>(xpos - lastX);
            float yoffset = static_cast<float>(ypos - lastY);
            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.2f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(direction);
        }

        // Toggle light position mapping with 'L' key
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            isLightPositionMappingEnabled = !isLightPositionMappingEnabled;  // Toggle the state

            if (isLightPositionMappingEnabled) {
                std::cout << "Light position mapping enabled." << std::endl;
            }
            else {
                std::cout << "Light position mapping disabled. Light at position: ("
                    << lightPos.x << ", " << lightPos.y << ")" << std::endl;
            }
        }

        // Change light color with R, G, B keys
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            lightColor = glm::vec3(0.7f, 0.3f, 0.3f); // Slightly red
            std::cout << "Light color changed to Slightly Red." << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            lightColor = glm::vec3(0.3f, 0.7f, 0.3f); // Slightly green
            std::cout << "Light color changed to Slightly Green." << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            lightColor = glm::vec3(0.3f, 0.3f, 0.7f); // Slightly blue
            std::cout << "Light color changed to Slightly Blue." << std::endl;
        }

        // Reset to white light using the 'W' key
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Reset to white light
            std::cout << "Light color reset to White." << std::endl;
        }

        // Handle robot movement (translation)
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (robotPosition.y > robotMaxY) {
                robotPosition.y = robotMaxY;
            }
            robotPosition.y += translationSpeed; // Update position directly
            
            if (isRobotView) {
                glm::vec3 robotHeadPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f);
                cameraPos = robotHeadPos;
            }
            std::cout << "Robot moved UP. New Position: " << robotPosition.x << ", " << robotPosition.y << ", " << robotPosition.z << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (robotPosition.y < robotMinY) {
                robotPosition.y = robotMinY;
            }
            robotPosition.y -= translationSpeed; // Update position directly
            
            if (isRobotView) {
                glm::vec3 robotHeadPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f);
                cameraPos = robotHeadPos;
            }
            std::cout << "Robot moved DOWN. New Position: " << robotPosition.x << ", " << robotPosition.y << ", " << robotPosition.z << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (robotPosition.x < robotMinX) {
                robotPosition.x = robotMinX;
            }
            robotPosition.x -= translationSpeed; // Update position directly
            
            if (isRobotView) {
                glm::vec3 robotHeadPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f);
                cameraPos = robotHeadPos;
            }
            
            std::cout << "Robot moved LEFT. New Position: " << robotPosition.x << ", " << robotPosition.y << ", " << robotPosition.z << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {

            if (robotPosition.x > robotMaxX) {
                robotPosition.x = robotMaxX;
            }
            robotPosition.x += translationSpeed; // Update position directly
            //if (yKeyPressed) {
            //    cameraPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f); // 假设机器人的位置
            //}
            if (isRobotView) {
                glm::vec3 robotHeadPos = robotPosition + glm::vec3(0.0f, 1.2f, 0.27f);
                cameraPos = robotHeadPos;
            }
            std::cout << "Robot moved RIGHT. New Position: " << robotPosition.x << ", " << robotPosition.y << ", " << robotPosition.z << std::endl;
        }


        // Toggle continuous rotation with 'Q'
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !qKeyPressed) {
            qKeyPressed = true; // Mark Q key as pressed
            isRotating = !isRotating; // Toggle rotation state
            if (!isRotating) {
                std::cout << "Rotation stopped.\n";
            }
            else {
                std::cout << "Rotation started.\n";
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
            qKeyPressed = false; // Mark Q key as released
        }

        // Zoom
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            zoomFactor -= zoomSpeed * deltaTime;
            if (zoomFactor < 0.1f) zoomFactor = 0.1f; // Prevent zoom from going too small
            std::cout << "Zooming IN. New Zoom Factor: " << zoomFactor << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            zoomFactor += zoomSpeed * deltaTime;
            if (zoomFactor > 3.0f) zoomFactor = 3.0f; // Prevent zoom from getting too large
            std::cout << "Zooming OUT. New Zoom Factor: " << zoomFactor << std::endl;
        }

        robotNode->transform = robotModelTransform;

        // Debugging output for final position of the robot
        std::cout << "Final robot position: " << robotModelTransform[3][0] << ", " << robotModelTransform[3][1] << ", " << robotModelTransform[3][2] << std::endl;
    }
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (isLightPositionMappingEnabled) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        // Map mouse position to light position range
        lightPos.x = (xpos / SCR_WIDTH) * 10.0f - 5.0f;  // X position between -5 and 5
        lightPos.y = -(ypos / SCR_HEIGHT) * 10.0f + 5.0f; // Y position between -5 and 5

        // Update light position in the shader
        Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
        shader.setVec3("light.position", lightPos);

        std::cout << "Mouse mapped to light position: (" << lightPos.x << ", " << lightPos.y << ")" << std::endl;
    }

    // Handle camera control when cursor is disabled
    if (!cursorDisabled) {
        firstMouse = true;
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}