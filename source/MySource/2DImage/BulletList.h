#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "../2DImage/Sprite.h"

#define IMG_BULLET_SIZE 1024

template <typename T>
std::vector<Texcoord> BulletTCList()
{
	std::vector<Texcoord> tcList;
	T bulletdata = {};
	tcList.resize(bulletdata.count);
	for (int i = 0; i < bulletdata.count; i++) {
		auto tc = MakeTexcoord(bulletdata.start.x + (bulletdata.size.x * i), bulletdata.start.y,
			bulletdata.size.x, bulletdata.size.y,
			IMG_BULLET_SIZE, IMG_BULLET_SIZE);
		tcList[i] = tc;
	}
	return tcList;
}


class RedBullet1
{
public:
	glm::vec2 start = { 384,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;	
};

class RedBullet2
{
public:
	glm::vec2 start = { 384,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class RedBullet3
{
public:
	glm::vec2 start = { 384,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class RedBullet4
{
public:
	glm::vec2 start = { 384,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class RedBullet5
{
public:
	glm::vec2 start = { 384,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class RedBullet6
{
public:
	glm::vec2 start = { 384,306 };
	glm::vec2 size = { 10,14 };
	int count = 5;
};

class RedBullet7
{
public:
	glm::vec2 start = { 384,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class RedBullet8
{
public:
	glm::vec2 start = { 384,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};

class BuleBullet1
{
public:
	glm::vec2 start = { 424,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class BuleBullet2
{
public:
	glm::vec2 start = { 424,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class BuleBullet3
{
public:
	glm::vec2 start = { 424,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class BuleBullet4
{
public:
	glm::vec2 start = { 444,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class BuleBullet5
{
public:
	glm::vec2 start = { 434,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class BuleBullet6
{
public:
	glm::vec2 start = { 434,306 };
	glm::vec2 size = { 10,14 };
	int count = 6;
};

class BuleBullet7
{
public:
	glm::vec2 start = { 428,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class BuleBullet8
{
public:
	glm::vec2 start = { 424,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};

class GreenBullet1
{
public:
	glm::vec2 start = { 464,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class GreenBullet2
{
public:
	glm::vec2 start = { 464,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class GreenBullet3
{
public:
	glm::vec2 start = { 464,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class GreenBullet4
{
public:
	glm::vec2 start = { 504,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class GreenBullet5
{
public:
	glm::vec2 start = { 484,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class GreenBullet6
{
public:
	glm::vec2 start = { 494,306 };
	glm::vec2 size = { 10,14 };
	int count = 6;
};

class GreenBullet7
{
public:
	glm::vec2 start = { 472,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class GreenBullet8
{
public:
	glm::vec2 start = { 464,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};

class YellowBullet1
{
public:
	glm::vec2 start = { 504,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class YellowBullet2
{
public:
	glm::vec2 start = { 504,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class YellowBullet3
{
public:
	glm::vec2 start = { 504,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class YellowBullet4
{
public:
	glm::vec2 start = { 564,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class YellowBullet5
{
public:
	glm::vec2 start = { 534,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class YellowBullet6
{
public:
	glm::vec2 start = { 554,306 };
	glm::vec2 size = { 10,14 };
	int count = 6;
};

class YellowBullet7
{
public:
	glm::vec2 start = { 512,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class YellowBullet8
{
public:
	glm::vec2 start = { 504,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};

class PurpleBullet1
{
public:
	glm::vec2 start = { 544,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class PurpleBullet2
{
public:
	glm::vec2 start = { 544,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class PurpleBullet3
{
public:
	glm::vec2 start = { 544,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class PurpleBullet4
{
public:
	glm::vec2 start = { 624,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class PurpleBullet5
{
public:
	glm::vec2 start = { 584,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class PurpleBullet6
{
public:
	glm::vec2 start = { 714,306 };
	glm::vec2 size = { 10,14 };
	int count = 6;
};

class PurpleBullet7
{
public:
	glm::vec2 start = { 560,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class PurpleBullet8
{
public:
	glm::vec2 start = { 544,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};
//
class LightBlueBullet1
{
public:
	glm::vec2 start = { 584,256 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class LightBlueBullet2
{
public:
	glm::vec2 start = { 584,266 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class LightBlueBullet3
{
public:
	glm::vec2 start = { 584,276 };
	glm::vec2 size = { 10,10 };
	int count = 4;
};

class LightBlueBullet4
{
public:
	glm::vec2 start = { 684,286 };
	glm::vec2 size = { 10,10 };
	int count = 6;
};

class LightBlueBullet5
{
public:
	glm::vec2 start = { 634,296 };
	glm::vec2 size = { 10,10 };
	int count = 5;
};

class LightBlueBullet6
{
public:
	glm::vec2 start = { 674,306 };
	glm::vec2 size = { 10,14 };
	int count = 6;
};

class LightBlueBullet7
{
public:
	glm::vec2 start = { 604,321 };
	glm::vec2 size = { 11,10 };
	int count = 4;
};

class LightBlueBullet8
{
public:
	glm::vec2 start = { 584,331 };
	glm::vec2 size = { 10,20 };
	int count = 4;
};

class LaserRedSmall
{
public:
	glm::vec2 start = { 320,0 };
	glm::vec2 size = { 23,512 };
	int count = 4;
};

class LaserBlueSmall
{
public:
	glm::vec2 start = { 412,0 };
	glm::vec2 size = { 23,512 };
	int count = 4;
};

class LaserPinkBig
{
public:
	glm::vec2 start = { 320,512 };
	glm::vec2 size = { 64,535 };
	int count = 7;
};

std::vector<Texcoord> SpiralBulletColor(int type);

std::vector<Texcoord> RandomBulletColor(int type);

std::vector<Texcoord> TwistBulletColor(int type);

std::vector<Texcoord> StraightLineBulletColor(int type);
