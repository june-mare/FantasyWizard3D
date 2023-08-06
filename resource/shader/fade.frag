/**
* @file fade.frag
*/
#version 450

// シェーダへの入力
layout(location=1) in vec2 inTexcoord;

// テクスチャ
layout(binding=0) uniform sampler2D texColor; // カラー画像
layout(binding=1) uniform sampler2D texRule;  // ルール画像

// ピクセルを破棄する「しきい値」
layout(location=100) uniform float threshold;

// 出力する色データ
out vec4 outColor;

void main()
{
  // ルール画像の値が「しきい値」未満だったら描画をキャンセル
  float a = texture(texRule, inTexcoord).r;
  if (a < threshold) {
    discard;
  }

  // 指定された画像を描画
  outColor = texture(texColor, inTexcoord);

  // 境界部分のぎざぎざ感をなくすために、アルファ値をなめらかに変化させる
  const float smoothingRange = 0.02;
  outColor.a *= smoothstep(threshold - smoothingRange, threshold, a);
}