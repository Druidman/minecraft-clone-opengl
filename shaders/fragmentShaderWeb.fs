#version 300 es
precision mediump float;


in vec2 TexCoords;
in vec3 playerStateColorChange;
out vec4 Color;

uniform sampler2D text;

void main()
{
    vec4 result = texture(text,TexCoords);
    Color = result + vec4(playerStateColorChange,0.0);
}