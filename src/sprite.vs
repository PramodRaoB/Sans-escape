#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 pos;

uniform mat4 model;
// note that we're omitting the view matrix; the view never changes so we basically have an identity view matrix and can therefore omit it.
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    pos = model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = projection * pos;
}