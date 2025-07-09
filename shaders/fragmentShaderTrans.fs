#version 330 core


in vec2 TexCoords;

out vec4 Color;

uniform sampler2D texture1;

void main()
{     
    Color = vec4(0.0,0.0,1.0,0.1);
}