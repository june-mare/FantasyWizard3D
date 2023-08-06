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

//爆発（物が壊れたとき）
class Effect1
{
public:
	glm::vec2 start = { 464,490 };
	glm::vec2 size = { 56,56 };
	int countY = 1,countX = 10;
};

//白い球
class Effect2
{
public:
	glm::vec2 start = { 576,960 };
	glm::vec2 size = { 32,32 };
	int countY = 2, countX = 14;
};
//竜巻
class Effect3
{
public:
	glm::vec2 start = { 576,928 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 13;
};
//白い雷玉
class Effect4
{
public:
	glm::vec2 start = { 576,896 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//土から爆発
class Effect5
{
public:
	glm::vec2 start = { 576,864 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};

//黄色雷
class Effect6
{
public:
	glm::vec2 start = { 576,832 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};

//炎
class Effect7
{
public:
	glm::vec2 start = { 576,800 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//水
class Effect8
{
public:
	glm::vec2 start = { 576,768 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//みどりの風
class Effect9
{
public:
	glm::vec2 start = { 576,736 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//黄色雷玉
class Effect10
{
public:
	glm::vec2 start = { 576,704 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//淡い黄色の球
class Effect11
{
public:
	glm::vec2 start = { 576,672 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 11;
};
//ステータスダウン
class Effect12
{
public:
	glm::vec2 start = { 576,640 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 10;
};
//ステータスアップ
class Effect13
{
public:
	glm::vec2 start = { 576,608 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 10;
};

//黒い煙
class Effect14
{
public:
	glm::vec2 start = { 576,576 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 8;
};
//真っ黒い炎
class Effect15
{
public:
	glm::vec2 start = { 576,544 };
	glm::vec2 size = { 32,32 };
	int countY = 1, countX = 9;
};
//img切り替え魔方陣1
class Effect16
{
public:
	glm::vec2 start = {0,0};
	glm::vec2 size = {64,64};
	int countY = 6, countX = 5;	
};

//出現オーラ
class Effect17
{
public:
	glm::vec2 start = { 0,384 };
	glm::vec2 size = { 64,64 };
	int countY = 2, countX = 5;
};
//魔方陣2
class Effect18
{
public:
	glm::vec2 start = { 0,512 };
	glm::vec2 size = { 64,64 };
	int countY = 5, countX = 5;
};

//魔方陣3
class Effect19
{
public:
	glm::vec2 start = { 0,832 };
	glm::vec2 size = { 64,64 };
	int countY = 4, countX = 5;
};
