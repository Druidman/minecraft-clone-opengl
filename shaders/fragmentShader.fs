#version 330 core


in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
out vec4 Color;

uniform sampler2D text;
uniform vec3 LightPos;

void main()
{
    vec3 lightColor = vec3(1.0,1.0,0.88);

    vec3 ambient = 0.1 * lightColor;
    vec3 objectColor = vec3(texture(text,TexCoords));
    


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - Pos) ;
    float diff = max(dot(norm,lightDir),0.0);

    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;

    float alpha = texture(text,TexCoords)[3];
    if (alpha < 1.0){
        alpha *= 0.5;
    }
    Color = vec4(result,alpha);
}