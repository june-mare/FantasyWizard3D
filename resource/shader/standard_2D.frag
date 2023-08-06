/**
* @file standard_2D.frag
*/
#version 450

layout(location=0)in vec4 inColor;
layout(location=1)in vec2 inTexcoord;

/*テクスチャサンプラ*/
layout(binding=0) uniform sampler2D texColor;

// 出力する色データ
out vec4 outColor;

void main()
{
  vec4 c = texture(texColor,inTexcoord);
  outColor = c * inColor;
//  outColor.rgb = inColor.rgb / 1000;
}