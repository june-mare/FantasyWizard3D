#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "Sprite.h"
template <typename T>

std::vector<Texcoord> EffectTCList()
{
	T effectdata;
	std::vector<Texcoord> tcList;
	int setNum = 0;
	tcList.resize(effectdata.countY * effectdata.countX);
	for (int i = 0; i < effectdata.countY; i++)
	{
		for (int j = 0; j < effectdata.countX; j++)
		{
			auto tc = MakeTexcoord(
				effectdata.start.x + (effectdata.size.x * j),
				effectdata.start.y + (effectdata.size.y * i),
				effectdata.size.x, effectdata.size.y,
				IMG_OBJECTS_SIZE, IMG_OBJECTS_SIZE);
			tcList[setNum] = tc;
			setNum++;
		}
	}
	return tcList;
}

//�����i������ꂽ�Ƃ��j
class Effect1
{
public:
	glm::vec2 start = { 464,490 };
	glm::vec2 size = { 56,56 };
	int countY = 1,countX = 10;
};

//������
class Effect2
{
public:
	glm::vec2 start = { 576,960 };
	glm::vec2 size = { 32,32 };
	int countY = 2, countX = 14;
};
//����
class Effect3
{
public:
	glm::vec2 start = { 576,928 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 13;
};
//��������
class Effect4
{
public:
	glm::vec2 start = { 576,896 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//�y���甚��
class Effect5
{
public:
	glm::vec2 start = { 576,864 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};

//���F��
class Effect6
{
public:
	glm::vec2 start = { 576,832 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};

//��
class Effect7
{
public:
	glm::vec2 start = { 576,800 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//��
class Effect8
{
public:
	glm::vec2 start = { 576,768 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//�݂ǂ�̕�
class Effect9
{
public:
	glm::vec2 start = { 576,736 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//���F����
class Effect10
{
public:
	glm::vec2 start = { 576,704 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//�W�����F�̋�
class Effect11
{
public:
	glm::vec2 start = { 576,672 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 11;
};
//�X�e�[�^�X�_�E��
class Effect12
{
public:
	glm::vec2 start = { 576,640 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 10;
};
//�X�e�[�^�X�A�b�v
class Effect13
{
public:
	glm::vec2 start = { 576,608 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 10;
};

//������
class Effect14
{
public:
	glm::vec2 start = { 576,576 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//�^��������
class Effect15
{
public:
	glm::vec2 start = { 576,544 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 9;
};
//img�؂�ւ������w1
class Effect16
{
public:
	glm::vec2 start = {0,0};
	glm::vec2 size = {64,64};
	int countY = 6, countX = 5;	
};

//�o���I�[��
class Effect17
{
public:
	glm::vec2 start = { 0,384 };
	glm::vec2 size = { 64,64 };
	int countY = 2, countX = 5;
};
//�����w2
class Effect18
{
public:
	glm::vec2 start = { 0,512 };
	glm::vec2 size = { 64,64 };
	int countY = 5, countX = 5;
};

//�����w3
class Effect19
{
public:
	glm::vec2 start = { 0,832 };
	glm::vec2 size = { 64,64 };
	int countY = 4, countX = 5;
};
