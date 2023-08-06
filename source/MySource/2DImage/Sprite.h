#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <string>
#define IMG_OBJECTS_SIZE 1088

/*図形に表示する範囲を表す構造体*/
	/*数値はテキスチャ座標系でしていする*/
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
	float u, v;	//テキスチャ座標
	float sx, sy;//画像の大きさ	
	bool operator==(const Texcoord& a)const
	{
		return u == a.u && v == a.v && sx == a.sx && sy == a.sy;
	}
};

/*画像表示用データ（スプライト）を格納する構造体*/
struct Sprite
{
	float x = 0.f, y = 0.f, z = 1.f, w = 1.f; /*画像を表示する座標*/
	Texcoord texcoord = {};	//テキスチャ座標と大きさ
	float angle = 1.0f / 180.f / 3.14f;
	float scale = 1.0f;
	float scaleX = 1.0f, scaleY = 1.0f;
	float alpha = 1.0f;
	float damy = 0.f;
};

/*スプライト配列型*/
using SpriteList = std::vector<Sprite>;

/*テクスチャ座標を作成する(座標はピクセル数で指定)*/
inline Texcoord MakeTexcoord(float u, float v, float sx, float sy) {
	return Texcoord{ u / 512, v / 512, sx / 512, sy / 512 };
};
inline Texcoord MakeTexcoord(float u, float v, float sx, float sy, float sizex, float sizey) {
	return Texcoord{ u / sizex, v / sizey, sx / sizex, sy / sizey };
};


bool WhichDiffDirection(side& vec, float x, float y);
bool WhichDiffDirection(side& vec, glm::vec3 position);
