/**
* @file shadow.vert
*/
#version 450

// シェーダへの入力
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexcoord;

// シェーダからの出力
out vec2 outTexcoord;
flat out int MaterialID;

struct MeshData{
    int[4] materialID;
    mat4 model;  
};

layout(std430,binding=2)readonly buffer meshDataBlock
{
  MeshData meshDataList[];
};

layout(location=1) uniform mat4 matShadow;

void main()
{
  outTexcoord = inTexcoord;
  MaterialID = meshDataList[gl_InstanceID].materialID[0];
  gl_Position = matShadow * (meshDataList[gl_InstanceID].model * vec4(inPosition, 1));
}