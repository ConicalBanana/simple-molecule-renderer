#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineSize;      // Outline size

void main()
{
    // Expand model along the normal direction
    vec3 pos = aPos + aNormal * outlineSize;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}