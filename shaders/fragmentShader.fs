#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D text;

void main()
{
    Color = texture(text, TexCoords);
}