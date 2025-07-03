#version 300 es

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aPosOffset;
layout (location = 4) in vec2 aTexOffset;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition + aPosOffset , 1.0);
    TexCoords = aUv + aTexOffset;
    Normal = aNormal;
    Pos = vec3(model * vec4(aPosition + aPosOffset , 1.0));
}