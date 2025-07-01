#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D texture8;

void main()
{
    Color = texture(texture8, TexCoords);
}