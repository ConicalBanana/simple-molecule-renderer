#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Model.hpp"

/*
extern constants
*/
// Screen size settings
extern const float SCR_WIDTH = 800.;
extern const float SCR_HEIGHT = 600.;

// View settings
// extern const float CUE_CUTOFF_FRONT = -2.0f;       // For a near clipping plane
extern const float CUE_CUTOFF_FRONT = -100.0f;  // For a far clipping plane
extern const float CUE_CUTOFF_BACK = 100.0f;

// Color settings
extern const GLfloat WHITE[3] = {1.0f, 1.0f, 1.0f};
extern const GLfloat GRAY[3] = {0.9f, 0.9f, 0.9f};
extern const GLfloat BLACK[3] = {0.0f, 0.0f, 0.0f};
extern const GLfloat RED[3] = {0.8f, 0.3f, 0.3f};
extern const GLfloat GREEN[3] = {0.3f, 0.8f, 0.3f};
extern const GLfloat ORANGE[3] = {0.8f, 0.5f, 0.0f};
extern const GLfloat* BACKGROUND_COLOR = WHITE;  // Canvas color
extern const bool OVERWRITE_COLOR = true;   // Whether overwriting the atomic color with the following colors
extern const glm::vec3 COLOR_LAYER_1 = glm::vec3(RED[0], RED[1], RED[2]);  // Color settings
extern const glm::vec3 COLOR_LAYER_2 = glm::vec3(GRAY[0], GRAY[1], GRAY[2]);
extern const glm::vec3 COLOR_LAYER_3 = glm::vec3(GRAY[0], GRAY[1], GRAY[2]);
extern const float ALPHA_LAYER_1 = 1.0f;  // Transparency settings, 1.0f for fully opaque
extern const float ALPHA_LAYER_2 = 0.3f;
extern const float ALPHA_LAYER_3 = 0.1f;
extern const unsigned int MODEL_MODE_LAYER_1 = MODEL_MODEL_CPK;  // Molecule model settings
extern const unsigned int MODEL_MODE_LAYER_2 = MODEL_MODEL_LINE;
extern const unsigned int MODEL_MODE_LAYER_3 = MODEL_MODEL_LINE;

// Toon shader settings
extern const float SHADOW_THRESHOLD = 0.3f;                                    // Boundary of light and shadow
extern const glm::vec3 SHADOW_COLOR = glm::vec3(GRAY[0], GRAY[1], GRAY[2]);    // Color of shadow
extern const float HIGHLIGHT_THRESHOLD = 1.0f;                                 // 1.0 for no highlight
extern const bool USE_DIRECTIONAL_LIGHT = true;                                // false = point light, true = directional light
extern const glm::vec3 POINT_LIGHT_POS = glm::vec3(1.2f, 1.0f, 2.0f);          // Point light position
extern const glm::vec3 DIRECTIONAL_LIGHT_DIR = glm::vec3(-0.5f, -0.5f, -0.5f); // Directional light direction

// Outline shader settings
extern const double OUTLINE_SIZE = 0.05;

// Export settings
extern const float HIGHR_RES_FACTOR = 4.0f; // 2x resolution
