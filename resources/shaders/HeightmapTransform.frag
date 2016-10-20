#version 330 core

in VS_OUT
{
    vec3 normal;
    vec2 texCoord;
} fs_in;

out vec4 color;

uniform sampler2D AlbedoTexture;
uniform sampler2D HeightmapTexture;

void main(void)
{
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 1.0f));
    vec3 diffuseLightColor = vec3(1.0f, 1.0f, 1.0f);

    float diffuse = dot(lightDirection, normalize(fs_in.normal));
    vec3 albedo = texture(AlbedoTexture, fs_in.texCoord).rgb;

    color = vec4(albedo * diffuseLightColor * diffuse, 1);
}

