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

uniform int NormalEstBaseX[4] = int[] (0, -1, 0, 1);
uniform int NormalEstBaseY[4] = int[] (-1, 0, 1, 0);

void main(void)
{
    float weirdsin = 0.0f;

    float height = texture(HeightmapTexture, texCoord1).r * texCoord2.x;
    vec3 displacement = vec3(0, height, 0);

    vec3 normal = vec3(0, 0, 0);
    vec2 texOffset = 1.0 / textureSize(HeightmapTexture, 0);
    for (int i = 0; i < 2; ++i)
    {
        vec2 uShift = vec2(NormalEstBaseX[2*i], NormalEstBaseY[2*i]);
        vec2 vShift = vec2(NormalEstBaseX[2*i+1], NormalEstBaseY[2*i+1]);

        float uHeight = texture(
            HeightmapTexture, texCoord1 + uShift * texOffset
        ).r;

        float vHeight = texture(
            HeightmapTexture, texCoord1 + vShift * texOffset
        ).r;

        normal += cross(
            vec3(uShift.x / (512-1), uHeight - height, uShift.y / (512-1)),
            vec3(vShift.x / (512-1), vHeight - height, vShift.y / (512-1))
        );
    }

    normal = normalize(normal);

    gl_Position = projection * view * model 
        * vec4(position + displacement, 1.0f);
    vs_out.texCoord = texCoord1;

    vs_out.normal =
        normalize((transpose(inverse(model)) * vec4(normal, 0)).xyz);
}
