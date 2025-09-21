#version 300 es
precision highp float;


in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

out vec4 Color;

uniform vec3 CameraPos;
uniform sampler2D text;
uniform vec3 LightPos;
uniform int playerState;

vec3 fogColor = vec3(0.68, 0.84, 0.9);

void main()
{
    if (Pos.x == -10000.0){
        discard;
    }
    vec3 lightColor = vec3(1.0,1.0,0.88);

    vec3 ambient = 0.1 * lightColor;
    vec4 objectColor = texture(text,TexCoords);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - Pos) ;
    float diff = max(dot(norm,lightDir),0.0);

    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * vec3(objectColor);
    
    float distance = length(vec2(Pos.x, Pos.z));
    float fogFactor = 1.0;
    
    float resultValue = -1.0 / (1.0 + pow(2.71 * 0.4, -1.0 * (distance - 240.0))) + 1.0;

    fogFactor = clamp(resultValue,0.0,1.0);
    
    
  
   if (objectColor[3] < 0.1){ // alpha discard
        discard;
    }
    if (playerState == 1){
        Color = vec4(result + vec3(0.0,0.069,0.247),objectColor[3]); // vec3(0.0,0.069,0.247)
    }
    else {
        Color = vec4(mix(fogColor, result, fogFactor),objectColor[3]);
    }
    
}