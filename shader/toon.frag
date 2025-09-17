#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // 环境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // 离散化漫反射光照 - 这是三渲二的关键
    float toonLevels = 4.0;
    float toonDiffuse = floor(diff * toonLevels) / toonLevels;
    vec3 diffuse = toonDiffuse * lightColor;
    
    // 镜面光 - 简化处理
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    // 离散化高光
    float toonSpec = (spec > 0.5) ? 1.0 : 0.0;
    vec3 specular = toonSpec * lightColor;
    
    // 组合结果
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}