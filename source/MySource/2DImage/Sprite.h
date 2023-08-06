#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <string>
#define IMG_OBJECTS_SIZE 1088

/*�}�`�ɕ\������͈͂�\���\����*/
	/*���l�̓e�L�X�`�����W�n�ł��Ă�����*/
enum class side
{
	non = 0,
	down = 3,
	left = 6,
	right = 9,
	up = 12,
};

struct Texcoord
{
	float u, v;	//�e�L�X�`�����W
	float sx, sy;//�摜�̑傫��	
	bool operator==(const Texcoord& a)const
	{
		return u == a.u && v == a.v && sx == a.sx && sy == a.sy;
	}
};

/*�摜�\���p�f�[�^�i�X�v���C�g�j���i�[����\����*/
struct Sprite
{
	float x = 0.f, y = 0.f, z = 1.f, w = 1.f; /*�摜��\��������W*/
	Texcoord texcoord = {};	//�e�L�X�`�����W�Ƒ傫��
	float angle = 1.0f / 180.f / 3.14f;
	float scale = 1.0f;
	float scaleX = 1.0f, scaleY = 1.0f;
	float alpha = 1.0f;
	float damy = 0.f;
};

/*�X�v���C�g�z��^*/
using SpriteList = std::vector<Sprite>;

/*�e�N�X�`�����W���쐬����(���W�̓s�N�Z�����Ŏw��)*/
inline Texcoord MakeTexcoord(float u, float v, float sx, float sy) {
	return Texcoord{ u / 512, v / 512, sx / 512, sy / 512 };
};
inline Texcoord MakeTexcoord(float u, float v, float sx, float sy, float sizex, float sizey) {
	return Texcoord{ u / sizex, v / sizey, sx / sizex, sy / sizey };
};


bool WhichDiffDirection(side& vec, float x, float y);
bool WhichDiffDirection(side& vec, glm::vec3 position);
