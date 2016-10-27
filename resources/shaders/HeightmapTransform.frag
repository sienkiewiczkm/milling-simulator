#version 330 core

in VS_OUT
{
    vec3 normal;
    vec2 texCoord;
} fs_in;

out vec4 color;

uniform sampler2D AlbedoTexture;
uniform sampler2D HeightmapTexture;

vec2 HeightmapSize;

void main(void)
{
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 1.0f));
    vec3 diffuseLightColor = vec3(1.0f, 1.0f, 1.0f);

    vec3 normal = normalize(fs_in.normal);
    float lightFactor = clamp(dot(lightDirection, normal), 0.0, 1.0);
    vec3 albedo = texture(AlbedoTexture, fs_in.texCoord).rgb;

    float adjustedLightFactor = clamp(lightFactor + 0.15, 0.0, 1.0);
    vec3 diffusePart = albedo * diffuseLightColor * adjustedLightFactor;

    color = vec4(diffusePart, 1);
}

