#version 330 core

out vec4 FragColor;

uniform float alpha;                    // Transparency of the outline

void main()
{
    // Black outline with transparency
    FragColor = vec4(0.0, 0.0, 0.0, alpha);
}