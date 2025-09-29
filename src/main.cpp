#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "ShapeGenerator.hpp"
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
void exportHighResPNG(GLFWwindow* window, const std::vector<model::Model>& models, 
                     unsigned int toonShader, unsigned int outlineShader);

/*
Constants
*/
// Screen size settings
const float SCR_WIDTH = 800.;
const float SCR_HEIGHT = 600.;

// Color settings
const GLfloat WHITE[3] = {1.0f, 1.0f, 1.0f};
const GLfloat GRAY[3] = {0.9f, 0.9f, 0.9f};
const GLfloat BLACK[3] = {0.0f, 0.0f, 0.0f};
const GLfloat RED[3] = {0.8f, 0.3f, 0.3f};
const GLfloat GREEN[3] = {0.3f, 0.8f, 0.3f};
const GLfloat* BACKGROUND_COLOR = WHITE;
const GLfloat* OBJECT_COLOR = GREEN;

// Toon shader settings
const float SHADOW_THRESHOLD = 0.3f;                                    // Boundary of light and shadow
const glm::vec3 SHADOW_COLOR = glm::vec3(GRAY[0], GRAY[1], GRAY[2]);    // Color of shadow
const float HIGHLIGHT_THRESHOLD = 1.0f;                                 // 1.0 for no highlight
const bool USE_DIRECTIONAL_LIGHT = true;                                // false = point light, true = directional light
const glm::vec3 POINT_LIGHT_POS = glm::vec3(1.2f, 1.0f, 2.0f);          // Point light position
const glm::vec3 DIRECTIONAL_LIGHT_DIR = glm::vec3(-0.5f, -0.5f, -0.5f); // Directional light direction
const float ALPHA = 1.0f;

// Outline shader settings
const double OUTLINE_SIZE = 0.05;

// Export settings
const float HIGHR_RES_FACTOR = 4.0f; // 2x resolution

/*
Global variables
*/
// Camera related
float orthoScalingFactor = 0.005f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Export control variable
bool exportRequested = false;

// Mouse and camera control variables
bool firstMouse = true;
bool mousePressed = false;
bool rightMousePressed = false;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;    // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
float pitch = 0.0f;
float zoom = 1.0f;

// Model rotation variables (separate from camera)
glm::mat4 modelRotation = glm::mat4(1.0f);

void setupRenderSettings(
    unsigned int shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::mat4& model,
    const glm::vec3& modelColor
) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Set lighting and color parameters for toon shader
    glUniform3fv(glGetUniformLocation(shader, "shadowColor"), 1, glm::value_ptr(SHADOW_COLOR));
    glUniform1f(glGetUniformLocation(shader, "highlightThreshold"), HIGHLIGHT_THRESHOLD);
    glUniform1f(glGetUniformLocation(shader, "shadowThreshold"), SHADOW_THRESHOLD);

    // Set lighting parameters
    if (USE_DIRECTIONAL_LIGHT) {
        glm::vec4 lightDir4 = glm::vec4(DIRECTIONAL_LIGHT_DIR, 0.0f);
        glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(glm::vec3(lightDir4)));
        glUniform1i(glGetUniformLocation(shader, "isDirectionalLight"), 1);
    } else {
        glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(POINT_LIGHT_POS));
        glUniform1i(glGetUniformLocation(shader, "isDirectionalLight"), 0);
    }

    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(cameraPos));
    glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, glm::value_ptr(modelColor));
    glUniform1f(glGetUniformLocation(shader, "alpha"), ALPHA);
}

void setupOutlineSettings(
    unsigned int shader,
    const glm::mat4& view, 
    const glm::mat4& projection,
    const glm::mat4& model
) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(glGetUniformLocation(shader, "outlineSize"), OUTLINE_SIZE);
    glUniform1f(glGetUniformLocation(shader, "alpha"), ALPHA);
}

void printDescription() {
    // Descriptions
    std::cout << "Shortcut description:" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    std::cout << "W/A/S/D: move camera" << std::endl;
    std::cout << "QE: move camera up/down" << std::endl;
    std::cout << "left mouse key: rotate view" << std::endl;
    std::cout << "right mouse key: rotate around camera vector" << std::endl;
    std::cout << "scroll wheel: zoom view" << std::endl;
    std::cout << "R: reset camera position" << std::endl;
    std::cout << "Ctrl+S: export PNG image (4x resolution)" << std::endl;
}

void setupBackground(void) {
    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


int main(int argc, char* argv[])
{
    // Parse command line arguments
    std::string filename = "./asset/C60-Ih.xyz";
    if (argc > 1) {
        std::string arg = argv[1];
        filename = arg;

        // -h or --help
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage:    " << argv[0] << " <filename>" << std::endl;
            std::cout << "Example:  " << argv[0] << " ./asset/C60-Ih.xyz" << std::endl;
            std::cout << "Shortcut: " << argv[0] << " -h or " << argv[0] << " --help" << std::endl;
            return 0;
        }
    }

    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);  // 4x MSAA

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
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                rightMousePressed = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {
                rightMousePressed = false;
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
    glEnable(GL_MULTISAMPLE);

    // Enable alpha blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printDescription();

    // Load multiple models
    chem::Xyz xyz = chem::Xyz(filename);
    xyz.autoCentering();
    std::vector<model::Model> models = model::loadMoleculeModel(xyz);

    // Load shaders
    unsigned int toonShader = loadShader("./src/shaders/toon.vert", "./src/shaders/toon.frag");
    unsigned int outlineShader = loadShader("./src/shaders/outline.vert", "./src/shaders/outline.frag");

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        setupBackground();

        // Create transformation matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::ortho(
            -SCR_WIDTH * orthoScalingFactor,
            SCR_WIDTH * orthoScalingFactor,
            -SCR_HEIGHT * orthoScalingFactor,
            SCR_HEIGHT * orthoScalingFactor,
            -100.0f, 100.0f
        );
        
        // Render all models
        for (const struct model::Model& model : models) {
            // Apply rotation around molecule center, then translate back to molecule center
            glm::mat4 finalTransform = modelRotation * model.transform;
            
            // First pass: render outline
            setupOutlineSettings(outlineShader, view, projection, finalTransform);
            glCullFace(GL_FRONT);
            glBindVertexArray(model.VAO);
            glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);

            // Second pass: render toon shading
            setupRenderSettings(toonShader, view, projection, finalTransform, model.color);
            glCullFace(GL_BACK);
            glBindVertexArray(model.VAO);
            glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
        }

        // Check if export is requested
        if (exportRequested) {
            exportHighResPNG(window, models, toonShader, outlineShader);
            exportRequested = false;
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

// Export high-resolution PNG image
void exportHighResPNG(GLFWwindow* window, const std::vector<model::Model>& models, 
                     unsigned int toonShader, unsigned int outlineShader)
{
    // Get current window size
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(window, &currentWidth, &currentHeight);
    
    // Calculate high-resolution size (4x resolution)
    int highResWidth = currentWidth * HIGHR_RES_FACTOR;
    int highResHeight = currentHeight * HIGHR_RES_FACTOR;
    
    // Create framebuffer for high-resolution rendering
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Create color texture
    unsigned int colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, highResWidth, highResHeight, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                          GL_TEXTURE_2D, colorTexture, 0);
    
    // Create depth renderbuffer
    unsigned int depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 
                         highResWidth, highResHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                             GL_RENDERBUFFER, depthRenderbuffer);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &colorTexture);
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        return;
    }
    
    // Set viewport for high-resolution rendering
    glViewport(0, 0, highResWidth, highResHeight);
    
    // Clear framebuffer
    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Create transformation matrices for high-resolution rendering
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::ortho(
        -(float)highResWidth * orthoScalingFactor / HIGHR_RES_FACTOR / 2,  // Scale down by 4x to match original view
        (float)highResWidth * orthoScalingFactor / HIGHR_RES_FACTOR / 2,
        -(float)highResHeight * orthoScalingFactor / HIGHR_RES_FACTOR / 2,
        (float)highResHeight * orthoScalingFactor / HIGHR_RES_FACTOR / 2,
        -100.0f, 100.0f
    );
    
    // Render all models at high resolution
    for (const auto& model : models) {
        glm::mat4 finalTransform = modelRotation * model.transform;
        
        // First pass: render outline
        setupOutlineSettings(outlineShader, view, projection, finalTransform);
        
        glCullFace(GL_FRONT);
        glBindVertexArray(model.VAO);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
        
        // Second pass: render toon shading
        setupRenderSettings(toonShader, view, projection, finalTransform, model.color);
        
        glCullFace(GL_BACK);
        glBindVertexArray(model.VAO);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
    }
    
    // Read pixels from framebuffer
    unsigned char* pixels = new unsigned char[highResWidth * highResHeight * 3];
    glReadPixels(0, 0, highResWidth, highResHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    // Flip image vertically (OpenGL renders upside down)
    unsigned char* flippedPixels = new unsigned char[highResWidth * highResHeight * 3];
    for (int y = 0; y < highResHeight; y++) {
        for (int x = 0; x < highResWidth; x++) {
            int srcIndex = (y * highResWidth + x) * 3;
            int dstIndex = ((highResHeight - 1 - y) * highResWidth + x) * 3;
            flippedPixels[dstIndex] = pixels[srcIndex];
            flippedPixels[dstIndex + 1] = pixels[srcIndex + 1];
            flippedPixels[dstIndex + 2] = pixels[srcIndex + 2];
        }
    }
    
    // Generate filename with timestamp
    std::time_t now = std::time(0);
    std::tm* timeinfo = std::localtime(&now);
    std::stringstream ss;
    ss << "molecule_export_" 
       << std::setfill('0') << std::setw(4) << (timeinfo->tm_year + 1900)
       << std::setfill('0') << std::setw(2) << (timeinfo->tm_mon + 1)
       << std::setfill('0') << std::setw(2) << timeinfo->tm_mday << "_"
       << std::setfill('0') << std::setw(2) << timeinfo->tm_hour
       << std::setfill('0') << std::setw(2) << timeinfo->tm_min
       << std::setfill('0') << std::setw(2) << timeinfo->tm_sec << ".png";
    std::string filename = ss.str();
    
    // Save PNG image
    if (stbi_write_png(filename.c_str(), highResWidth, highResHeight, 3, flippedPixels, highResWidth * 3)) {
        std::cout << "High-resolution PNG exported successfully: " << filename 
                  << " (" << highResWidth << "x" << highResHeight << ")" << std::endl;
    } else {
        std::cout << "Failed to export PNG image" << std::endl;
    }
    
    // Clean up
    delete[] pixels;
    delete[] flippedPixels;
    
    // Restore original framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWidth, currentHeight);
    
    // Delete framebuffer objects
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &colorTexture);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
}

// Process input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Export PNG with Ctrl+S
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
         glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) &&
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        exportRequested = true;
    }
    
    // Camera movement with WASD keys
    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && 
        !(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
          glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    
    // Reset camera position with R key
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        zoom = 1.0f;
        orthoScalingFactor = 0.005f;
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
    if (!mousePressed && !rightMousePressed)
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

    if (mousePressed) {
        // Left mouse: Standard model rotation around world axes
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Apply rotations to the model rotation matrix
        modelRotation = rotationY * rotationX * modelRotation;
    } else if (rightMousePressed) {
        // Right mouse: Rotation around camera vector (roll rotation)
        // Use horizontal mouse movement for camera vector rotation
        float rollAngle = xoffset;
        
        // Rotate around the camera's front vector (camera vector)
        glm::mat4 rollRotation = glm::rotate(glm::mat4(1.0f), rollAngle, cameraFront);
        
        // Apply roll rotation to the model rotation matrix
        modelRotation = rollRotation * modelRotation;
    }
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
    orthoScalingFactor = 0.005f / zoom;
}
