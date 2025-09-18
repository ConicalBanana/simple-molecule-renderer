#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "SimpleShapeGenerator.hpp"
#include "Element.hpp"
#include "Xyz.hpp"
#include "Model.hpp"

/*
Function declaration
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadShader(const char* vertexPath, const char* fragmentPath);
/*
Constants
*/
// Screen size settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float ORTHO_SCALING_FACTOR = 0.005f;  // Remove const to allow zoom

// Color settings
const GLfloat WHITE[3] = {1.0f, 1.0f, 1.0f};
const GLfloat GRAY[3] = {0.9f, 0.9f, 0.9f};
const GLfloat BLACK[3] = {0.0f, 0.0f, 0.0f};
const GLfloat RED[3] = {0.8f, 0.3f, 0.3f};
const GLfloat GREEN[3] = {0.3f, 0.8f, 0.3f};
const GLfloat* BACKGROUND_COLOR = WHITE;
const GLfloat* OBJECT_COLOR = GREEN;

// Camera settings - Remove const to allow interaction
glm::vec3 CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CAMERA_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);

// Mouse and camera control variables
bool firstMouse = true;
bool mousePressed = false;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;    // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
float pitch = 0.0f;
float zoom = 1.0f;

// Model rotation variables (separate from camera)
glm::mat4 modelRotation = glm::mat4(1.0f);
const glm::vec3 VIEW_CENTER = glm::vec3(0.0f);

// Shadow settings
const float SHADOW_THRESHOLD = 0.3f;                                 // Boundary of light and shadow
const glm::vec3 SHADOW_COLOR = glm::vec3(GRAY[0], GRAY[1], GRAY[2]); // Color of shadow

// Highlight settings
const float HIGHLIGHT_THRESHOLD = 1.0f; // 1.0 for no highlight

// Light settings
const bool USE_DIRECTIONAL_LIGHT = true;                               // false = point light, true = directional light
const glm::vec3 POINT_LIGHT_POS = glm::vec3(1.2f, 1.0f, 2.0f);          // Point light position
const glm::vec3 DIRECTIONAL_LIGHT_DIR = glm::vec3(-0.5f, -0.5f, -0.5f); // Directional light direction

// Outline settings
const double OUTLINE_SIZE = 0.05;

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enable anti-aliasing with 4x MSAA
    glfwWindowHint(GLFW_SAMPLES, 16);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Toon Shading Example", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                mousePressed = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {
                mousePressed = false;
                firstMouse = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    });
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Enable multisampling for anti-aliasing
    glEnable(GL_MULTISAMPLE);
    
    // Check if MSAA is supported and report the sample count
    GLint samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    if (samples > 0) {
        std::cout << "Anti-aliasing enabled with " << samples << "x MSAA" << std::endl;
    } else {
        std::cout << "Warning: Anti-aliasing not supported on this system" << std::endl;
    }
    
    // Descriptions
    std::cout << "Shortcut description:" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    std::cout << "W/A/S/D: move camera" << std::endl;
    std::cout << "QE: move camera up/down" << std::endl;
    std::cout << "left mouse key: rotate view" << std::endl;
    std::cout << "scroll wheel: zoom view" << std::endl;
    std::cout << "R: reset camera position" << std::endl;
    
    // Load shaders
    unsigned int toonShader = loadShader("./src/shaders/toon.vert", "./src/shaders/toon.frag");
    unsigned int outlineShader = loadShader("./src/shaders/outline.vert", "./src/shaders/outline.frag");

    // Load multiple models
    chem::Xyz xyz = chem::Xyz("./asset/ps.xyz");

    // Get geometric center of molecule
    std::array<double, 3> geom_center = xyz.getGeomCenter();

    // Apply centering to the molecule data BEFORE creating the models
    for (size_t i = 0; i < xyz.atomCoordArray.size(); i++) {
        xyz.atomCoordArray[i][0] -= geom_center[0];
        xyz.atomCoordArray[i][1] -= geom_center[1];
        xyz.atomCoordArray[i][2] -= geom_center[2];
    }

    // Now load the models with the centered coordinates
    std::vector<model::Model> models = model::loadMoleculeModel(xyz);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // 输入处理
        processInput(window);
        
        // 渲染
        glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Create transformation matrices
        glm::mat4 view = glm::lookAt(CAMERA_POS, CAMERA_POS + CAMERA_FRONT, CAMERA_UP);
        glm::mat4 projection = glm::ortho(
            -(float)SCR_WIDTH * ORTHO_SCALING_FACTOR,
            (float)SCR_WIDTH * ORTHO_SCALING_FACTOR,
            -(float)SCR_HEIGHT * ORTHO_SCALING_FACTOR,
            (float)SCR_HEIGHT * ORTHO_SCALING_FACTOR,
            -100.0f, 100.0f
        );
        
        // Render all models
        for (const auto& model : models) {
            // Apply rotation around molecule center, then translate back to molecule center
            glm::mat4 finalTransform = modelRotation * model.transform;
            
            // First pass: render outline
            glUseProgram(outlineShader);
            glUniformMatrix4fv(glGetUniformLocation(outlineShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(outlineShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(outlineShader, "model"), 1, GL_FALSE, glm::value_ptr(finalTransform));
            glUniform1f(glGetUniformLocation(outlineShader, "outlineSize"), OUTLINE_SIZE);
            
            glCullFace(GL_FRONT);
            glBindVertexArray(model.VAO);
            glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
            
            // Second pass: render toon shading
            glUseProgram(toonShader);
            glUniformMatrix4fv(glGetUniformLocation(toonShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(toonShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(toonShader, "model"), 1, GL_FALSE, glm::value_ptr(finalTransform));
            
            // Set lighting and color parameters
            glUniform3fv(glGetUniformLocation(toonShader, "shadowColor"), 1, glm::value_ptr(SHADOW_COLOR));
            glUniform1f(glGetUniformLocation(toonShader, "highlightThreshold"), HIGHLIGHT_THRESHOLD);
            glUniform1f(glGetUniformLocation(toonShader, "shadowThreshold"), SHADOW_THRESHOLD);
            glUniform3fv(glGetUniformLocation(toonShader, "SHADOW_COLOR"), 1, glm::value_ptr(SHADOW_COLOR));
            
            // Set lighting parameters (light remains fixed in world space)
            if (USE_DIRECTIONAL_LIGHT) {
                glm::vec4 lightDir4 = glm::vec4(DIRECTIONAL_LIGHT_DIR, 0.0f);
                glUniform3fv(glGetUniformLocation(toonShader, "lightPos"), 1, glm::value_ptr(glm::vec3(lightDir4)));
                glUniform1i(glGetUniformLocation(toonShader, "isDirectionalLight"), 1);
            } else {
                glUniform3fv(glGetUniformLocation(toonShader, "lightPos"), 1, glm::value_ptr(POINT_LIGHT_POS));
                glUniform1i(glGetUniformLocation(toonShader, "isDirectionalLight"), 0);
            }
            
            glUniform3fv(glGetUniformLocation(toonShader, "viewPos"), 1, glm::value_ptr(CAMERA_POS));
            glUniform3f(glGetUniformLocation(toonShader, "lightColor"), 1.0f, 1.0f, 1.0f);
            
            // Set object color
            glUniform3fv(glGetUniformLocation(toonShader, "objectColor"), 1, glm::value_ptr(model.color));
            
            glCullFace(GL_BACK);
            glBindVertexArray(model.VAO);
            glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
        }
        
        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up resources
    model::cleanupModels(models);
    glDeleteProgram(toonShader);
    glDeleteProgram(outlineShader);
    
    glfwTerminate();
    return 0;
}

// Process input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Camera movement with WASD keys
    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CAMERA_POS += cameraSpeed * CAMERA_FRONT;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CAMERA_POS -= cameraSpeed * CAMERA_FRONT;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        CAMERA_POS -= glm::normalize(glm::cross(CAMERA_FRONT, CAMERA_UP)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        CAMERA_POS += glm::normalize(glm::cross(CAMERA_FRONT, CAMERA_UP)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        CAMERA_POS += cameraSpeed * CAMERA_UP;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        CAMERA_POS -= cameraSpeed * CAMERA_UP;
    
    // Reset camera position with R key
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        CAMERA_POS = glm::vec3(0.0f, 0.0f, 3.0f);
        CAMERA_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
        CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        zoom = 1.0f;
        ORTHO_SCALING_FACTOR = 0.005f;
        modelRotation = glm::mat4(1.0f);  // Reset model rotation
    }
}

// Window size adjustment callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int loadShader(const char* vertexPath, const char* fragmentPath)
{
    // Read vertex shader
    FILE* file = fopen(vertexPath, "r");
    if (!file) {
        std::cout << "ERROR::SHADER::VERTEX::FILE_NOT_READ" << std::endl;
        return 0;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* vertexCode = new char[length + 1];
    fread(vertexCode, 1, length, file);
    vertexCode[length] = '\0';
    fclose(file);
    
    // Read fragment shader
    file = fopen(fragmentPath, "r");
    if (!file) {
        std::cout << "ERROR::SHADER::FRAGMENT::FILE_NOT_READ" << std::endl;
        delete[] vertexCode;
        return 0;
    }
    
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* fragmentCode = new char[length + 1];
    fread(fragmentCode, 1, length, file);
    fragmentCode[length] = '\0';
    fclose(file);
    
    // Compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Shader program
    unsigned int id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    delete[] vertexCode;
    delete[] fragmentCode;
    
    return id;
}

// Mouse movement callback for model rotation (not camera rotation)
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!mousePressed)
        return;
        
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.01f;  // Reduced sensitivity for model rotation
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Create rotation matrices for model rotation around molecule center
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Apply rotations to the model rotation matrix
    modelRotation = rotationY * rotationX * modelRotation;
}

// Scroll callback for zooming
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    zoom -= (float)yoffset * 0.1f;
    if (zoom < 0.1f)
        zoom = 0.1f;
    if (zoom > 10.0f)
        zoom = 10.0f;
        
    // Update orthographic scaling factor for zoom effect
    ORTHO_SCALING_FACTOR = 0.005f / zoom;
}
