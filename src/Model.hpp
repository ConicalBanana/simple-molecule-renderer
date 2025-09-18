#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "SimpleShapeGenerator.hpp"
#include "Molecule.hpp"
#include "Element.hpp"


const float VDWR_SCALING_RATIO = 0.2f;
const float BOND_RADIUS = 0.05f;
const int ATOM_MODEL_RESOLUTION = 8;
const int BOND_MODEL_RESOLUTION = 8;

namespace model{
    // Add this structure to hold model data
    struct Model {
        unsigned int VAO;
        unsigned int VBO;
        int vertexCount;
        glm::mat4 transform;
        glm::vec3 color;
        
        Model() : VAO(0), VBO(0), vertexCount(0), 
                transform(glm::mat4(1.0f)), 
                color(glm::vec3(0.3f, 0.8f, 0.3f)) {}
    };

    void renderModel(const Model& model, unsigned int shader, const glm::mat4& view, const glm::mat4& );
    void cleanupModels(std::vector<Model>& models);
    Model loadAtomModel(const unsigned int& atom_number, const std::array<double, 3>& atom_coord);
    Model loadBondModel(const std::array<double, 6>& bond_vec);
    std::vector<model::Model> loadMoleculeModel(chem::MoleculeFile& moleculeFile);
}

void model::renderModel(
    const Model& model,
    unsigned int shader, 
    const glm::mat4& view,
    const glm::mat4& 
) {
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model.transform));
    
    // Set object color if it's the toon shader
    GLint colorLocation = glGetUniformLocation(shader, "objectColor");
    if (colorLocation != -1) {
        glUniform3fv(colorLocation, 1, glm::value_ptr(model.color));
    }
    
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
}

model::Model model::loadAtomModel(
    const unsigned int& atom_number,
    const std::array<double, 3>& atom_coord
) {
    float sphere_radius = chem::VDWR_ARRAY[atom_number] * VDWR_SCALING_RATIO;
    std::array<float, 3> sphere_color = chem::COLOR_ARRAY[atom_number - 1];

    model::Model sphere;
    std::vector<float> vertices = SphereGenerator::generateVertices(sphere_radius, ATOM_MODEL_RESOLUTION*2, ATOM_MODEL_RESOLUTION);
    
    glGenVertexArrays(1, &sphere.VAO);
    glGenBuffers(1, &sphere.VBO);
    
    glBindVertexArray(sphere.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    sphere.vertexCount = vertices.size() / 6;
    sphere.transform = glm::translate(glm::mat4(1.0f), glm::vec3(atom_coord[0], atom_coord[1], atom_coord[2]));
    // sphere.transform = glm::rotate(sphere.transform, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    // sphere.color = glm::vec3(0.3f, 0.8f, 0.3f); // Green
    sphere.color = glm::vec3(sphere_color[0], sphere_color[1], sphere_color[2]);

    return sphere;
}

model::Model model::loadBondModel(
    const std::array<double, 6>& bond_vec
) {
    model::Model cylinder;
    
    // Calculate bond vector and length
    glm::vec3 start = glm::vec3(bond_vec[0], bond_vec[1], bond_vec[2]);
    glm::vec3 end = glm::vec3(bond_vec[3], bond_vec[4], bond_vec[5]);
    glm::vec3 bondVector = end - start;
    float bondLength = glm::length(bondVector);
    
    // Generate cylinder vertices with appropriate radius and the calculated length
    float bondRadius = BOND_RADIUS;
    std::vector<float> vertices = CylinderGenerator::generateVertices(
        bondRadius, bondLength, BOND_MODEL_RESOLUTION*2, BOND_MODEL_RESOLUTION
    );
    
    // Create OpenGL buffers
    glGenVertexArrays(1, &cylinder.VAO);
    glGenBuffers(1, &cylinder.VBO);
    
    glBindVertexArray(cylinder.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                &vertices[0], GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    cylinder.vertexCount = vertices.size() / 6;
    
    // Calculate transformation matrix to position and orient the cylinder
    glm::vec3 midpoint = (start + end) * 0.5f;  // Center of the bond
    
    // Create rotation matrix to align cylinder with bond vector
    glm::vec3 defaultDirection = glm::vec3(0.0f, 0.0f, 1.0f);  // Cylinder default direction
    glm::vec3 bondDirection = glm::normalize(bondVector);
    
    glm::mat4 transform = glm::mat4(1.0f);
    
    // Translate to midpoint
    transform = glm::translate(transform, midpoint);
    
    // Rotate to align with bond direction
    // Check if we need to rotate at all
    float dotProduct = glm::dot(defaultDirection, bondDirection);
    if (abs(dotProduct) < 0.999f) {  // Not parallel or anti-parallel
        glm::vec3 rotationAxis = glm::normalize(glm::cross(defaultDirection, bondDirection));
        float rotationAngle = acos(glm::clamp(dotProduct, -1.0f, 1.0f));
        transform = glm::rotate(transform, rotationAngle, rotationAxis);
    } else if (dotProduct < 0) {
        // Vectors are opposite (180-degree rotation needed)
        // Choose an arbitrary perpendicular axis for 180-degree rotation
        glm::vec3 perpAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        if (abs(glm::dot(defaultDirection, perpAxis)) > 0.9f) {
            perpAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        transform = glm::rotate(transform, glm::pi<float>(), perpAxis);
    }

    cylinder.transform = transform;
    cylinder.color = glm::vec3(0.7f, 0.7f, 0.7f);  // Gray color for bonds
    
    return cylinder;
}

std::vector<model::Model> model::loadMoleculeModel(chem::MoleculeFile& moleculeFile){
    std::vector<model::Model> models;

    const size_t atom_count = moleculeFile.size();
    for (size_t i = 0; i < atom_count; i++){
        models.push_back(
            model::loadAtomModel(
                moleculeFile.atomNumberArray[i],
                moleculeFile.atomCoordArray[i]
            )
        );
    }

    const std::vector<std::array<double, 6>> bond_vector_array =
        moleculeFile.getBondVectorArray();
    for (size_t i = 0; i < bond_vector_array.size(); i++){
        models.push_back(model::loadBondModel(bond_vector_array[i]));
    }

    glBindVertexArray(0);
    return models;
}


void model::cleanupModels(std::vector<Model>& models) {
    for (auto& model : models) {
        glDeleteVertexArrays(1, &model.VAO);
        glDeleteBuffers(1, &model.VBO);
    }
    models.clear();
}
