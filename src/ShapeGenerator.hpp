#pragma once
#include <vector>
#include <cmath>

/**
 * SphereGenerator - Creates sphere vertices equivalent to gluSphere
 * 
 * This class generates sphere vertices that can be used with modern OpenGL
 * to replace the deprecated gluSphere function.
 */
class SphereGenerator {
public:
    /**
     * Generate sphere vertices as a flat array for glDrawArrays
     * 
     * @param radius Sphere radius
     * @param slices Number of horizontal subdivisions (longitude)
     * @param stacks Number of vertical subdivisions (latitude)
     * @return Vector of floats: [x,y,z,nx,ny,nz, x,y,z,nx,ny,nz, ...]
     */
    static std::vector<float> generateVertices(float radius, int slices, int stacks) {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        
        // Generate all vertices first
        std::vector<float> allVertices;
        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = PI / 2.0f - i * PI / stacks;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);
            
            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = j * 2.0f * PI / slices;
                
                // Position
                float x = xy * cosf(sliceAngle);
                float y = xy * sinf(sliceAngle);
                
                // Normal (normalized position for unit sphere)
                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;
                
                allVertices.insert(allVertices.end(), {x, y, z, nx, ny, nz});
            }
        }
        
        // Generate triangles
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;
                
                // First triangle
                addVertex(vertices, allVertices, first);
                addVertex(vertices, allVertices, second);
                addVertex(vertices, allVertices, first + 1);
                
                // Second triangle
                addVertex(vertices, allVertices, second);
                addVertex(vertices, allVertices, second + 1);
                addVertex(vertices, allVertices, first + 1);
            }
        }
        
        return vertices;
    }
    
    /**
     * Generate sphere with texture coordinates
     * 
     * @param radius Sphere radius
     * @param slices Number of horizontal subdivisions
     * @param stacks Number of vertical subdivisions
     * @return Vector of floats: [x,y,z,nx,ny,nz,u,v, ...]
     */
    static std::vector<float> generateVerticesWithTexCoords(float radius, int slices, int stacks) {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        
        // Generate all vertices first
        std::vector<float> allVertices;
        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = PI / 2.0f - i * PI / stacks;
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);
            
            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = j * 2.0f * PI / slices;
                
                // Position
                float x = xy * cosf(sliceAngle);
                float y = xy * sinf(sliceAngle);
                
                // Normal
                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;
                
                // Texture coordinates
                float u = (float)j / slices;
                float v = (float)i / stacks;
                
                allVertices.insert(allVertices.end(), {x, y, z, nx, ny, nz, u, v});
            }
        }
        
        // Generate triangles
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;
                
                // First triangle
                addVertexWithTexCoords(vertices, allVertices, first);
                addVertexWithTexCoords(vertices, allVertices, second);
                addVertexWithTexCoords(vertices, allVertices, first + 1);
                
                // Second triangle
                addVertexWithTexCoords(vertices, allVertices, second);
                addVertexWithTexCoords(vertices, allVertices, second + 1);
                addVertexWithTexCoords(vertices, allVertices, first + 1);
            }
        }
        
        return vertices;
    }
    
    /**
     * Get the number of vertices that will be generated
     */
    static int getVertexCount(int slices, int stacks) {
        return slices * stacks * 6;  // 2 triangles per quad, 3 vertices per triangle
    }
    
private:
    static void addVertex(std::vector<float>& dest, const std::vector<float>& src, int index) {
        int start = index * 6;
        dest.insert(dest.end(), src.begin() + start, src.begin() + start + 6);
    }
    
    static void addVertexWithTexCoords(std::vector<float>& dest, const std::vector<float>& src, int index) {
        int start = index * 8;
        dest.insert(dest.end(), src.begin() + start, src.begin() + start + 8);
    }
};


/**
 * CylinderGenerator - Creates cylinder vertices equivalent to gluCylinder
 * 
 * This class generates cylinder vertices that can be used with modern OpenGL
 * to replace the deprecated gluCylinder function.
 */
class CylinderGenerator {
public:
    /**
     * Generate cylinder vertices as a flat array for glDrawArrays
     * 
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param slices Number of horizontal subdivisions (around circumference)
     * @param stacks Number of vertical subdivisions (along height)
     * @return Vector of floats: [x,y,z,nx,ny,nz, x,y,z,nx,ny,nz, ...]
     */
    static std::vector<float> generateVertices(float radius, float height, int slices, int stacks) {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        
        // Generate all vertices first
        std::vector<float> allVertices;
        for (int i = 0; i <= stacks; ++i) {
            float z = height / 2.0f - i * height / stacks;
            
            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = j * 2.0f * PI / slices;
                
                // Position
                float x = radius * cosf(sliceAngle);
                float y = radius * sinf(sliceAngle);
                
                // Normal (pointing outward from cylinder axis)
                float nx = cosf(sliceAngle);
                float ny = sinf(sliceAngle);
                float nz = 0.0f;  // Cylinder sides have no z-component in normal
                
                allVertices.insert(allVertices.end(), {x, y, z, nx, ny, nz});
            }
        }
        
        // Generate triangles for cylinder sides
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;
                
                // First triangle
                addVertex(vertices, allVertices, first);
                addVertex(vertices, allVertices, second);
                addVertex(vertices, allVertices, first + 1);
                
                // Second triangle
                addVertex(vertices, allVertices, second);
                addVertex(vertices, allVertices, second + 1);
                addVertex(vertices, allVertices, first + 1);
            }
        }
        
        return vertices;
    }
    
    /**
     * Generate cylinder with texture coordinates
     * 
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param slices Number of horizontal subdivisions
     * @param stacks Number of vertical subdivisions
     * @return Vector of floats: [x,y,z,nx,ny,nz,u,v, ...]
     */
    static std::vector<float> generateVerticesWithTexCoords(float radius, float height, int slices, int stacks) {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        
        // Generate all vertices first
        std::vector<float> allVertices;
        for (int i = 0; i <= stacks; ++i) {
            float z = height / 2.0f - i * height / stacks;
            
            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = j * 2.0f * PI / slices;
                
                // Position
                float x = radius * cosf(sliceAngle);
                float y = radius * sinf(sliceAngle);
                
                // Normal
                float nx = cosf(sliceAngle);
                float ny = sinf(sliceAngle);
                float nz = 0.0f;
                
                // Texture coordinates
                float u = (float)j / slices;
                float v = (float)i / stacks;
                
                allVertices.insert(allVertices.end(), {x, y, z, nx, ny, nz, u, v});
            }
        }
        
        // Generate triangles for cylinder sides
        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;
                
                // First triangle
                addVertexWithTexCoords(vertices, allVertices, first);
                addVertexWithTexCoords(vertices, allVertices, second);
                addVertexWithTexCoords(vertices, allVertices, first + 1);
                
                // Second triangle
                addVertexWithTexCoords(vertices, allVertices, second);
                addVertexWithTexCoords(vertices, allVertices, second + 1);
                addVertexWithTexCoords(vertices, allVertices, first + 1);
            }
        }
        
        return vertices;
    }
    
    /**
     * Generate cylinder with caps (top and bottom circles)
     * 
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param slices Number of horizontal subdivisions
     * @param stacks Number of vertical subdivisions
     * @return Vector of floats: [x,y,z,nx,ny,nz, ...]
     */
    static std::vector<float> generateVerticesWithCaps(float radius, float height, int slices, int stacks) {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        
        // First generate the cylinder sides
        std::vector<float> sideVertices = generateVertices(radius, height, slices, stacks);
        vertices.insert(vertices.end(), sideVertices.begin(), sideVertices.end());
        
        // Generate top cap (z = height/2)
        float topZ = height / 2.0f;
        for (int j = 0; j < slices; ++j) {
            float angle1 = j * 2.0f * PI / slices;
            float angle2 = (j + 1) * 2.0f * PI / slices;
            
            // Center vertex
            vertices.insert(vertices.end(), {0.0f, 0.0f, topZ, 0.0f, 0.0f, 1.0f});
            
            // Edge vertices
            vertices.insert(vertices.end(), {
                radius * cosf(angle1), radius * sinf(angle1), topZ, 0.0f, 0.0f, 1.0f
            });
            vertices.insert(vertices.end(), {
                radius * cosf(angle2), radius * sinf(angle2), topZ, 0.0f, 0.0f, 1.0f
            });
        }
        
        // Generate bottom cap (z = -height/2)
        float bottomZ = -height / 2.0f;
        for (int j = 0; j < slices; ++j) {
            float angle1 = j * 2.0f * PI / slices;
            float angle2 = (j + 1) * 2.0f * PI / slices;
            
            // Center vertex
            vertices.insert(vertices.end(), {0.0f, 0.0f, bottomZ, 0.0f, 0.0f, -1.0f});
            
            // Edge vertices (reversed order for correct winding)
            vertices.insert(vertices.end(), {
                radius * cosf(angle2), radius * sinf(angle2), bottomZ, 0.0f, 0.0f, -1.0f
            });
            vertices.insert(vertices.end(), {
                radius * cosf(angle1), radius * sinf(angle1), bottomZ, 0.0f, 0.0f, -1.0f
            });
        }
        
        return vertices;
    }
    
    /**
     * Get the number of vertices that will be generated (sides only)
     */
    static int getVertexCount(int slices, int stacks) {
        return slices * stacks * 6;  // 2 triangles per quad, 3 vertices per triangle
    }
    
    /**
     * Get the number of vertices that will be generated (with caps)
     */
    static int getVertexCountWithCaps(int slices, int stacks) {
        return slices * stacks * 6 + slices * 6;  // sides + top cap + bottom cap
    }
    
private:
    static void addVertex(std::vector<float>& dest, const std::vector<float>& src, int index) {
        int start = index * 6;
        dest.insert(dest.end(), src.begin() + start, src.begin() + start + 6);
    }
    
    static void addVertexWithTexCoords(std::vector<float>& dest, const std::vector<float>& src, int index) {
        int start = index * 8;
        dest.insert(dest.end(), src.begin() + start, src.begin() + start + 8);
    }
};