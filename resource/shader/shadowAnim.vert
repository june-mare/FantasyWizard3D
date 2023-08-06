/**
* @file shadow.vert
*/
#version 450

// シェーダへの入力
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aRotate;
layout (location = 4) in vec3 aBitRotate;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aWeights;


const int MAX_BONES = 82;
const int MAX_BONE_INFLUENCE = 4;

// シェーダからの出力
out vec2 outTexcoord;
flat out int MaterialID;

struct AnimData{
    int[4] materialID;
    mat4 model;  
    mat4[MAX_BONES] finalMatrix;
};

layout(std430,binding=1)readonly buffer animDataBlock
{
  AnimData animDataList[];
};

// プログラムからの入力
layout(location=1) uniform mat4 matShadow;

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
        //vec3 localNormal = mat3(animDataList[gl_InstanceID].finalMatrix[aBoneIds[i]]) * aNormal;
   }
  MaterialID =  animDataList[gl_InstanceID].materialID[0];
  outTexcoord = aTexCoords;
  gl_Position = matShadow * (animDataList[gl_InstanceID].model * totalPosition);
}