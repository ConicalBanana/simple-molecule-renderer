# Point Light vs Directional Light Guide

## Overview
This guide explains the differences between point lights and directional lights in 3D rendering, and how to toggle between them in the molecule renderer.

## Light Types

### 1. Point Light (Default)
- **Position**: Has a specific position in 3D space
- **Direction**: Light rays emanate in all directions from the source
- **Attenuation**: Light intensity decreases with distance
- **Real-world example**: Light bulb, candle, lamp

**Characteristics:**
- Objects closer to the light appear brighter
- Objects farther from the light appear dimmer
- Shadows vary based on object distance from light
- Creates more dramatic lighting effects

### 2. Directional Light
- **Position**: No specific position (infinitely far away)
- **Direction**: All light rays are parallel
- **Attenuation**: No distance-based attenuation
- **Real-world example**: Sunlight, moonlight

**Characteristics:**
- All objects receive the same light intensity
- Shadows are consistent regardless of object position
- Creates uniform lighting across the scene
- Simpler and more predictable lighting

## Controls

| Key | Function |
|-----|----------|
| **P** | Toggle between Point Light and Directional Light |
| **W** | Toggle white shadow effect |
| **↑↓** | Adjust light threshold |
| **←→** | Adjust shadow intensity |
| **ESC** | Exit program |

## Technical Implementation

### Point Light Calculation
```glsl
// Light direction from fragment to light source
vec3 lightDir = normalize(lightPos - FragPos);

// Distance attenuation
float distance = length(lightPos - FragPos);
float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
```

### Directional Light Calculation
```glsl
// Fixed light direction (parallel rays)
vec3 lightDir = normalize(-lightPos);  // lightPos stores direction

// No attenuation
float attenuation = 1.0;
```

## Visual Differences

### Point Light Effects
- **Gradient lighting**: Objects show smooth light-to-dark transitions
- **Distance falloff**: Closer surfaces are brighter
- **Radial shadows**: Shadows emanate from the light source
- **Localized illumination**: Only nearby objects are well-lit

### Directional Light Effects
- **Uniform lighting**: Consistent brightness across all surfaces
- **Parallel shadows**: All shadows point in the same direction
- **No falloff**: Distance doesn't affect brightness
- **Global illumination**: All objects receive equal lighting

## Configuration Parameters

### Point Light Settings
```cpp
glm::vec3 pointLightPos = glm::vec3(1.2f, 1.0f, 2.0f);  // Light position
```

### Directional Light Settings
```cpp
glm::vec3 directionalLightDir = glm::vec3(-0.5f, -0.5f, -0.5f);  // Light direction
```

## Use Cases

### When to Use Point Light
- **Indoor scenes**: Room lighting, desk lamps
- **Dramatic effects**: Spotlights, fire light
- **Local illumination**: Highlighting specific objects
- **Realistic lighting**: When simulating real light sources

### When to Use Directional Light
- **Outdoor scenes**: Sunlight simulation
- **Consistent lighting**: When uniform illumination is needed
- **Performance**: Simpler calculations, better performance
- **Stylized rendering**: Clean, predictable lighting

## Shader Uniforms

The shader receives these uniforms to handle both light types:

```glsl
uniform vec3 lightPos;              // Position (point) or direction (directional)
uniform bool isDirectionalLight;    // Light type flag
uniform vec3 viewPos;              // Camera position
uniform vec3 lightColor;           // Light color
uniform vec3 objectColor;          // Object base color
```

## White Shadow Interaction

Both light types work with the white shadow effect:

### Point Light + White Shadow
- Creates dramatic contrast between lit and shadow areas
- Shadow boundaries change based on object distance
- More dynamic and varied shadow patterns

### Directional Light + White Shadow
- Creates consistent shadow boundaries
- More predictable and uniform results
- Better for stylized, cartoon-like effects

## Performance Considerations

### Point Light
- **More calculations**: Distance and attenuation computation
- **Variable complexity**: Performance depends on scene geometry
- **Memory usage**: Slightly higher due to additional calculations

### Directional Light
- **Fewer calculations**: No distance or attenuation computation
- **Consistent performance**: Same cost regardless of scene complexity
- **Memory efficient**: Minimal additional overhead

## Debugging Tips

1. **Toggle between modes**: Use 'P' key to compare effects
2. **Adjust thresholds**: Use arrow keys to fine-tune lighting
3. **Check console output**: Monitor current light type
4. **Experiment with positions**: Modify light parameters in code

## Advanced Features

The enhanced shader (`toon_enhanced.frag`) provides additional features:
- **Proper attenuation**: Distance-based light falloff for point lights
- **Better specular**: More realistic highlight calculations
- **Quantized lighting**: Multiple lighting levels for toon shading
- **Flexible parameters**: Runtime control over all lighting aspects

## Customization

You can modify the lighting parameters in the code:

```cpp
// Adjust point light position
pointLightPos = glm::vec3(x, y, z);

// Adjust directional light direction
directionalLightDir = glm::vec3(x, y, z);

// Modify attenuation (in shader)
float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
```
