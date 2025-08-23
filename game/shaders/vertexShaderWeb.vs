#version 300 es
#extension GL_ANGLE_multi_draw : require
precision highp float;


layout (location = 0) in vec3 aBasePosition;
layout (location = 1) in float vertexType;
layout (location = 2) in float vertexData; 
layout (location = 3) in int chunkIndex;

layout(std140) uniform ubo{
    vec4 chunkPositions[1024];
};


out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 CameraPos;
    
vec3 rotateVertexPosition(vec3 pos, int face) {
    if (face == 0) { // top (Y+)
        pos.y += 0.5;
        return pos;
    }
    if (face == 1) { // bottom (Y-)
        pos.y -= 0.5;
        return pos;
    }
    if (face == 2) { // front (Z+)
        // Rotate 90° around X axis: (y,z) → (z,-y)
        

        float y = pos.y;
        pos.y = -pos.z;
        pos.z = y;
        pos.z += 0.5;
        return pos;
    }
    if (face == 3) { // back (Z-)
        // Rotate -90° around X axis: (y,z) → (-z,y)
        float y = pos.y;
        pos.y = pos.z;
        pos.z = -y;
        pos.z -= 0.5;
        return pos;
    }
    if (face == 4) { // left (X-)
        // Rotate 90° around Z axis: (x,y) → (y,-x)
        float x = pos.x;
        pos.x = pos.y;
        pos.y = -x;

        pos.x -= 0.5;
        return pos;
    }
    if (face == 5) { // right (X+)
        // Rotate -90° around Z axis: (x,y) → (-y,x)
        float x = pos.x;
        pos.x = -pos.y;
        pos.y = x;
        pos.x += 0.5;
        return pos;
    }

    return pos; // fallback
}

vec2 getUV(int vertexType, int face) {
    // vertex type is int 0,1,2,3

    // 0: 0.0, 0.75, bottom left
    // 1: 0.0, 1.0,  top left
    // 2: 0.25, 1.0, top right
    // 3: 0.25, 0.75 bottom right

    if (face == 0) { 
        // top (Y+) 
    }
    else if (face == 1) { 
        // bottom (Y-) 
    }
    else if (face == 2) { 
        // front (Z+) 
    }
    else if (face == 3) { 
        // back
        if (vertexType == 0) { 
            vertexType = 2; 
        }
        else if (vertexType == 1) { 
            vertexType = 3; 
        }
        else if (vertexType == 2) { 
            vertexType = 0; 
        }
        else if (vertexType == 3) { 
            vertexType = 1; 
        }
    }
    else if (face == 4) { 
        // left (X-)
        if (vertexType == 0) { 
            vertexType = 2; 
        }
        else if (vertexType == 1) { 
            vertexType = 1; 
        }
        else if (vertexType == 2) { 
            vertexType = 0; 
        }
        else if (vertexType == 3) { 
            vertexType = 3; 
        }
        
    }
    else if (face == 5) { 
        // right (X+)
        if (vertexType == 0) { 
            vertexType = 0; 
        }
        else if (vertexType == 1) { 
            vertexType = 3; 
        }
        else if (vertexType == 2) { 
            vertexType = 2; 
        }
        else if (vertexType == 3) { 
            vertexType = 1; 
        }
    }

    vec2 uvCoord;
    if (vertexType == 0){
        uvCoord = vec2(0.0, 0.9);
    }
    else if (vertexType == 1){
        uvCoord = vec2(0.0, 1.0);
    }
    else if (vertexType == 2){
        uvCoord = vec2(0.25, 1.0);
    }
    else if (vertexType == 3){
        uvCoord = vec2(0.25, 0.9);
    }
    return uvCoord;

}

vec3 NORMALS[6] = vec3[](
    vec3(0.0,1.0,0.0),
    vec3(0.0,-1.0,0.0),
    vec3(0.0,0.0,1.0),
    vec3(0.0,0.0,-1.0),
    vec3(-1.0,0.0,0.0),
    vec3(1.0,0.0,0.0)
    
);

void main()
{
    // check if active
    int intBits = floatBitsToInt(vertexData);

    int face = (intBits >> 16) & 0x7;
    if (face == 6){ // this face does not exist
        TexCoords = vec2(0.0);
        Normal = vec3(0.0, 1.0, 0.0); 
        Pos = vec3(0.0);
        gl_Position = vec4(-100.0); // degenerate position
        return ;
    }
    // Decode position from bit-packed int
    
    float zPos = float((intBits >> 0) & 0xF) + 0.5;
    float yPos = float((intBits >> 4) & 0xFF) + 0.5;
    float xPos = float((intBits >> 12) & 0xF) + 0.5;

  
    int textureId = (intBits >> 19) & 0x7F;
    vec3 rotatedBasePos = rotateVertexPosition(aBasePosition,face);
    

    vec3 blockOffset = vec3(xPos, yPos, zPos);
    vec3 chunkPos = vec3(chunkPositions[gl_DrawID]);
    vec3 worldPosition = rotatedBasePos + chunkPos + blockOffset - vec3(8.0,0.0,8.0) + CameraPos;

    gl_Position = projection * view * model * vec4(worldPosition, 1.0);
    Pos = vec3(model * vec4(worldPosition, 1.0));
    Normal = NORMALS[face];


    float yChange = - ( float( textureId / 4 ) * 0.1 );
    float xChange = (float(textureId) / 4.0) - float(textureId / 4);
    
    vec2 vertexUvs = getUV(int(vertexType),face);
    TexCoords = vertexUvs + vec2(xChange,yChange); 
}