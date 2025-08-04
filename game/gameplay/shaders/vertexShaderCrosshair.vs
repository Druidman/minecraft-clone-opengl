#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPosition;


void main()
{
    gl_Position = vec4(aPosition , 1.0);
    
}