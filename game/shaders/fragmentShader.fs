#version 460 core



in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;


out vec4 Color;

uniform sampler2D text;
uniform vec3 LightPos;
uniform int playerState;

void main()
{
    vec3 lightColor = vec3(1.0,1.0,0.88);

    vec3 ambient = 0.1 * lightColor;
    vec4 objectColor = texture(text,TexCoords);
    
    
    vec3 lightDir = normalize(LightPos - Pos);
    float diff = max(dot(Normal,lightDir),0.0);

    vec3 diffuse = diff * lightColor;

    

    // vec3 viewDir = normalize(vec3(0.0) - Pos);
    // vec3 reflectDir = reflect(-lightDir, Normal);  

    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // vec3 specular = 0.5 * spec * lightColor;  

    vec3 result = (ambient + diffuse) * vec3(objectColor);
  
    if (objectColor[3] < 0.1){ // alpha discard
        discard;
    }
    if (playerState == 1){
        Color = vec4(result + vec3(0.0,0.069,0.247),objectColor[3]);
    }
    else {
        Color = vec4(result,objectColor[3]);
    }
    
}