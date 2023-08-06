#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout (location=0)out vec3 FragPos;
layout (location=1)out vec3 Normal;
layout (location=2)out vec2 TexCoords;
layout (location=3)flat out int MaterialID;
layout (location=4)out vec3 outShadowPosition;

uniform mat4 shadowMat;

struct MeshData{
    int[4] materialID;
    mat4 model;  
};

layout(std430,binding=2)readonly buffer meshDataBlock
{
  MeshData meshDataList[];
};

struct Camera
{
   vec3 position;
   float dammy;
   mat4 projection;
   mat4 view;
};

layout(std430,binding=7)readonly buffer cameraDataBlock
{
  Camera camera;
};

void main()
{
    TexCoords = aTexCoords;
    MaterialID = meshDataList[gl_InstanceID].materialID[0];
    Normal = mat3(transpose(inverse(meshDataList[gl_InstanceID].model))) * aNormal;  
    gl_Position = camera.projection * camera.view * meshDataList[gl_InstanceID].model * vec4(aPos, 1.0);
    FragPos = gl_Position.xyz; 
    outShadowPosition = vec3(shadowMat * vec4(FragPos, 1));
}