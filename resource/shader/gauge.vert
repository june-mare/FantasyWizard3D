/**
*@file standerd_2D.vert
*/
#version 450 
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexcoord;

layout(location=0) out vec2 outTexcoord;

layout(location=1)uniform vec3  position;
layout(location=2)uniform vec4  screenToCrip;
layout(location=3)uniform vec4  texcoord;

layout(binding=0) uniform sampler2D texColor;

mat2 Rotate(float angle)
{	
	return mat2(
	cos(angle), -sin(angle),
	sin(angle), cos(angle));
}

void main()
{
 /*�e�N�X�`�����W���v�Z*/
 outTexcoord = texcoord.xy + (inTexcoord * texcoord.zw);

 /*���_���W���v�Z*/
 gl_Position = vec4(inPosition, 1);

 /*�}�`�̑傫�����摜�̑傫���Ɠ���������*/
 gl_Position.xy *=  abs(texcoord.zw) * textureSize(texColor, 0);
 
 /*�}�`��SSBO�̍��W�Ɉړ�*/
 gl_Position.xyz += position;//

 /*��ʍ��W�n����N���b�v���W�n�֕ϊ�*/
 gl_Position.xy = (gl_Position.xy / (screenToCrip.xy * 0.5)) + screenToCrip.zw;
}