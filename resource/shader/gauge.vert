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
 /*テクスチャ座標を計算*/
 outTexcoord = texcoord.xy + (inTexcoord * texcoord.zw);

 /*頂点座標を計算*/
 gl_Position = vec4(inPosition, 1);

 /*図形の大きさを画像の大きさと等しくする*/
 gl_Position.xy *=  abs(texcoord.zw) * textureSize(texColor, 0);
 
 /*図形をSSBOの座標に移動*/
 gl_Position.xyz += position;//

 /*画面座標系からクリップ座標系へ変換*/
 gl_Position.xy = (gl_Position.xy / (screenToCrip.xy * 0.5)) + screenToCrip.zw;
}