#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord1;
layout (location = 3) in vec2 texCoord2;

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
uniform mat4 TextureMatrix;

uniform vec3 HeightmapSize;

uniform int NormalEstBaseX[4] = int[] (0, -1, 0, 1);
uniform int NormalEstBaseY[4] = int[] (-1, 0, 1, 0);

void main(void)
{
    float height = texture(HeightmapTexture, texCoord1).r * texCoord2.x;
    vec3 displacement = vec3(0, height, 0);

    vec2 autoTexCoord = (TextureMatrix * vec4(position, 1)).xz;
    vec2 finalTexCoord = (texCoord2.y) * autoTexCoord
        + (1.0 - texCoord2.y) * texCoord1;

    vec3 heightNormal = vec3(0, 0, 0);
    vec2 texOffset = 1.0 / textureSize(HeightmapTexture, 0);

    for (int i = 0; i < 2; ++i)
    {
        vec2 uShift = vec2(NormalEstBaseX[2*i], NormalEstBaseY[2*i]);
        vec2 vShift = vec2(NormalEstBaseX[2*i+1], NormalEstBaseY[2*i+1]);

        float uHeight = texture(
            HeightmapTexture, finalTexCoord + uShift * texOffset
        ).r;

        float vHeight = texture(
            HeightmapTexture, finalTexCoord + vShift * texOffset
        ).r;

        heightNormal += cross(
            vec3(uShift.x / (512-1), uHeight - height, uShift.y / (512-1)),
            vec3(vShift.x / (512-1), vHeight - height, vShift.y / (512-1))
        );
    }

    vec3 finalNormal = (texCoord2.y)*normalize(heightNormal)
        + (1.0-texCoord2.y) * normal;

    gl_Position = projection * view * model 
        * vec4(HeightmapSize * position 
               + displacement, 1.0f);

    vs_out.texCoord = finalTexCoord;

    vs_out.normal =
        normalize((transpose(inverse(model)) * vec4(finalNormal, 0)).xyz);
}
