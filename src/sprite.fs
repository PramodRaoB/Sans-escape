#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;
uniform vec3 spriteColor;

in vec4 pos;
uniform int light;
uniform vec4 lightPos;

void main()
{
//     float dist = distance(pos, lightPos);
float dist = length(pos - lightPos);
    if (light == 0 && dist > 150.0f) {
//         color = vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
color = vec4(0.0f);
    }
    else {
        color = vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
    }
}