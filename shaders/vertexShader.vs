#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aPosOffset;
layout (location = 3) in vec2 aTexOffset;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition + aPosOffset , 1.0);
    TexCoords = aUv + aTexOffset;
}