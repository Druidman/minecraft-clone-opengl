

#version 330 core

layout (location = 0) in vec3 aBasePosition;
layout (location = 1) in int vertexData;  // <- Input int directly

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 chunkPos;


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



vec3 NORMALS[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, -1.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 1.0, 0.0)
    
);

void main()
{
    // Decode position from bit-packed int
    float zPos = float((vertexData >> 0) & 0xF) + 0.5;
    float yPos = float((vertexData >> 4) & 0xFF) + 0.5;
    float xPos = float((vertexData >> 12) & 0xF) + 0.5;

    int face = (vertexData >> 16) & 0x7;
    int textureId = (vertexData >> 19) & 0x7F;
    vec3 rotatedBasePos = rotateVertexPosition(aBasePosition,face);

    vec3 blockOffset = vec3(xPos, yPos, zPos);
    vec3 worldPosition = rotatedBasePos + chunkPos + blockOffset - vec3(8.0,0.0,8.0);

    gl_Position = projection * view * model * vec4(worldPosition, 1.0);
    Pos = vec3(model * vec4(worldPosition, 1.0));
    Normal = NORMALS[face];

    TexCoords = vec2(0.0, 0.0); // placeholder
}