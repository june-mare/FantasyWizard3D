/**
* @file fade.frag
*/
#version 450

// �V�F�[�_�ւ̓���
layout(location=1) in vec2 inTexcoord;

// �e�N�X�`��
layout(binding=0) uniform sampler2D texColor; // �J���[�摜
layout(binding=1) uniform sampler2D texRule;  // ���[���摜

// �s�N�Z����j������u�������l�v
layout(location=100) uniform float threshold;

// �o�͂���F�f�[�^
out vec4 outColor;

void main()
{
  // ���[���摜�̒l���u�������l�v������������`����L�����Z��
  float a = texture(texRule, inTexcoord).r;
  if (a < threshold) {
    discard;
  }

  // �w�肳�ꂽ�摜��`��
  outColor = texture(texColor, inTexcoord);

  // ���E�����̂������������Ȃ������߂ɁA�A���t�@�l���Ȃ߂炩�ɕω�������
  const float smoothingRange = 0.02;
  outColor.a *= smoothstep(threshold - smoothingRange, threshold, a);
}