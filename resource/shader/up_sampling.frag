/*
*up_sampling.frag
*/
#version 450

// シェーダへの入力
layout(location=1) in vec2 inTexcoord;

// テクスチャ
layout(binding=0) uniform sampler2D textureDiff;

// 出力する色データ
out vec4 outColor;

/**
* エントリーポイント
*/
void main()
{
  // テクセルサイズを計算
  vec2 halfTexel = 0.5 / vec2(textureSize(textureDiff, 0));

  // 3x3ピクセルのぼかし処理を行う
  outColor  = texture(textureDiff, inTexcoord + vec2(-halfTexel.x, halfTexel.y));
  outColor += texture(textureDiff, inTexcoord + vec2( halfTexel.x, halfTexel.y));
  outColor += texture(textureDiff, inTexcoord + vec2( halfTexel.x,-halfTexel.y));
  outColor += texture(textureDiff, inTexcoord + vec2(-halfTexel.x,-halfTexel.y));
  outColor *= 1.0 / 4.0; // 平均化
}