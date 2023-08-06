/**
*@file standerd_2D.vert
*/

#version 450 
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexcoord;


layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexcoord;

layout(location=0)uniform float timer;
layout(location=1)uniform vec3  position;

layout(location=2)uniform vec4 screenToCrip;

layout(binding=0) uniform sampler2D texColor;

struct StTexcoord
{
	vec2 uv;
	vec2 size;
};


/*��ʕ\���p�f�[�^���i�[����\����*/
struct StSprite
{
  float[4] position;
  //vec4 position;
  StTexcoord texcoord;
  float angle;
  float scale;
  float scaleX;
  float scaleY;
  float alpha;
 // float damy;
 // float damy2;
};

mat2 Rotate(float angle)
{	
	return mat2(
	cos(angle), -sin(angle),
	sin(angle), cos(angle)
	);
}

mat2 Scale(float scale)
{
	return mat2(
	scale ,0.0f,
	0.0f,scale
	);
}

mat2 Scale(float scalex,float scaley)
{
	return mat2(
	scalex ,0.0f,
	0.0f,scaley
	);
}


/*�X�v���C�g�p��SSBO*/
layout(std430,binding=0) buffer  spriteDataBlock
{
  StSprite spriteList[];
};

void main()
{
 outColor = vec4(1,1,1,spriteList[gl_InstanceID].alpha);

 /*�e�N�X�`�����W���v�Z*/
 StTexcoord texcoord = spriteList[gl_InstanceID].texcoord;
 outTexcoord = texcoord.uv + (inTexcoord * texcoord.size);

 /*���_���W���v�Z*/
 gl_Position = vec4(inPosition, 1);

 /*�}�`�̑傫�����摜�̑傫���Ɠ���������*/
 gl_Position.xy *=  abs(texcoord.size) * textureSize(texColor, 0);

 /*��]�ƃX�P�[���̏���*/
 gl_Position.xy = Rotate(spriteList[gl_InstanceID].angle) * gl_Position.xy;
 gl_Position.xy = Scale(spriteList[gl_InstanceID].scale) * gl_Position.xy;
 gl_Position.xy = Scale(spriteList[gl_InstanceID].scaleX ,spriteList[gl_InstanceID].scaleY) * gl_Position.xy;

 /*�}�`��SSBO�̍��W�Ɉړ�*/
 gl_Position.xyz += vec3(
		spriteList[gl_InstanceID].position[0],
		spriteList[gl_InstanceID].position[1],
		spriteList[gl_InstanceID].position[2] );//

 /*��ʍ��W�n����N���b�v���W�n�֕ϊ�*/
 gl_Position.xy = (gl_Position.xy / (screenToCrip.xy * 0.5)) + screenToCrip.zw;
}