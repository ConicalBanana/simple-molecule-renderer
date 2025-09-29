#version 330 core

in vec3 Normal;     // Normal vector of the fragment
in vec3 FragPos;    // Position of the fragment

out vec4 FragColor;

uniform vec3 lightPos;                  // Position of the light
uniform vec3 viewPos;                   // Position of the camera
uniform vec3 lightColor;                // Color of the light
uniform vec3 objectColor;               // Color of the object
uniform bool isDirectionalLight;        // True for directional light, False for point light
uniform vec3 shadowColor;               // color < shadowThreshold, use shadowColor;
uniform float highlightThreshold;       // Top highlight threshold; 1 for no highlight.
uniform float shadowThreshold;          // Boundary of light and shadow
uniform float alpha;                    // Transparency of the object

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    
    // Light vectors, see also:
    // https://zhuanlan.zhihu.com/p/427477685
    if (isDirectionalLight) {
        lightDir = normalize(-lightPos);            // Parallel light
    } else {
        lightDir = normalize(lightPos - FragPos);   // Point light
    }
    // Diff. coeff.(DC)
    // In most case, the final coefficient should be
    // DC * mater. DC * light. DC
    // In this model, only the basic part is used.
    float diffIntensity = max(dot(norm, lightDir), 0.0);
    
    // Calculate color
    vec3 result;
    if (diffIntensity > shadowThreshold) {
        // Highlight effects. Use specular light model
        float shininess = 32.0;
        float highlightColor = 0.3;

        vec3 viewDir = normalize(viewPos - FragPos);
        // genType reflect(	genType I, genType N);
        // I: Specifies the incident vector.
        // N: Specifies the normal vector.
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        // > hightlightThreshold: highlightColor
        // <= hightlightThreshold: no additional highlight
        float toonSpec = (spec > highlightThreshold) ? highlightColor : 0.0;

        // Add highlight color to object color
        result = objectColor + vec3(toonSpec);
    } else {
        // Given shadow color, irrelevant to object color
        result = shadowColor;
    }

    FragColor = vec4(result, alpha);
}