#version 330 core

in vec3 Normal;
in vec2 TexCoord;
out vec4 color;

uniform sampler2D TextureSlot1;

void main(void)
{
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 1.0f));
    vec3 diffuseLightColor = vec3(1.0f, 1.0f, 1.0f);
    float diffuse = dot(lightDirection, normalize(Normal));
    vec3 albedo = texture(TextureSlot1, TexCoord).rgb;
    color = vec4(albedo * diffuseLightColor * diffuse, 1);
}

