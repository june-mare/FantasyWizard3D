/**
* @file shadow.frag
*/
#version 450

// �V�F�[�_�ւ̓���
in vec2 inTexcoord;
flat in int MaterialID;

// �e�N�X�`��
layout (binding=0)uniform sampler2D texture_diffuse1;
layout (binding=1)uniform sampler2D texture_specular1;
layout (binding=2)uniform sampler2D texture_normal1;

struct Material{
   vec4 baseColor; 
   float roughness;	
   float shininess;
   float matallic;	
   vec3 specular;
};
layout(std430,binding=3)readonly buffer MaterialDataBlock
{
  Material materialList[];
};


void main()
{
  float alpha = texture(texture_diffuse1, inTexcoord).a * materialList[MaterialID].baseColor.a;
  if (alpha < 0.5) {
    discard;
  }
}