#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D TextureSlot1;

void main(void)
{
    color = texture(TextureSlot1, TexCoord);
}

