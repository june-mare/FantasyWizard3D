/*
*AssimpAnimation.vert
*/
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aRotate;
layout (location = 4) in vec3 aBitRotate;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aWeights;

//uniform mat4 projection;
//uniform mat4 view;

const int MAX_BONES = 82;
const int MAX_BONE_INFLUENCE = 4;

layout (location=0)out vec3 FragPos;
layout (location=1)out vec3 Normal;
layout (location=2)out vec2 TexCoords;
layout (location=3)flat out int MaterialID;
layout (location=4)out vec3 outShadowPosition;
//layout (location=5)out mat3 TBN;
//layout (location=6)out vec3 TotalNormal;

uniform mat4 shadowMat;

struct AnimData{
    int[4] materialID;
    mat4 model;  
    mat4[MAX_BONES] finalMatrix;
};

layout(std430,binding=1)readonly buffer animDataBlock
{
  AnimData animDataList[];
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
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIds[i] == -1) 
            continue;
        if(aBoneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = 
        animDataList[gl_InstanceID].finalMatrix[aBoneIds[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * aWeights[i];
   }
	  
    mat4 viewModel = camera.view * animDataList[gl_InstanceID].model;
    gl_Position =  camera.projection * viewModel * totalPosition;

    MaterialID = animDataList[gl_InstanceID].materialID[0];
	TexCoords = aTexCoords;
    FragPos = gl_Position.xyz;
    outShadowPosition = vec3(shadowMat * vec4(FragPos, 1));
    Normal = mat3(transpose(inverse(animDataList[gl_InstanceID].model))) * aNormal;   
}