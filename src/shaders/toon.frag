#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform bool isDirectionalLight;
uniform vec3 shadowColor;
uniform float highlightThreshold;  // Top highlight threshold; 1 for no highlight.
uniform float shadowThreshold;       // Boundary of light and shadow

void main()
{
    // 计算法线和光照方向
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    
    if (isDirectionalLight) {
        // 平行光：光照方向是固定的
        lightDir = normalize(-lightPos);  // lightPos实际存储的是光照方向
    } else {
        // 点光源：光照方向从片段位置指向光源位置
        lightDir = normalize(lightPos - FragPos);
    }
    
    float NdotL = max(dot(norm, lightDir), 0.0);
    
    // 设置光照阈值 - 可以调整这个值来控制阴影边界
    // float shadowThreshold = 0.25;
    
    // 根据光照强度决定颜色
    vec3 result;
    if (NdotL > shadowThreshold) {
        // 受光部分：使用物体原色
        
        // 可选：添加简单的高光效果
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        float toonSpec = (spec > highlightThreshold) ? 0.3 : 0.0; // 降低高光强度
        
        result = objectColor + vec3(toonSpec);
    } else {
        // 阴影部分：纯白色
        result = shadowColor;
    }
    
    FragColor = vec4(result, 1.0);
}