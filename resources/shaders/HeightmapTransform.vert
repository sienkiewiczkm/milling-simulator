#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord1;
layout (location = 2) in vec2 texCoord2;

out VS_OUT
{
    vec3 normal;
    vec2 texCoord;
} vs_out;

uniform sampler2D AlbedoTexture;
uniform sampler2D HeightmapTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
    float weirdsin = 0.0f;
    vec3 displacement = vec3(0, weirdsin, 0);
    gl_Position = projection * view * model 
        * vec4(position + displacement, 1.0f);
    vs_out.texCoord = texCoord1;
    vs_out.normal = (transpose(inverse(model)) * vec4(0, 1, 0, 0)).xyz;
}
