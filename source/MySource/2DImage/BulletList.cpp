#include "BulletList.h"

std::vector<Texcoord> SpiralBulletColor(int type)
{
	std::vector<Texcoord> tcList;
	switch (type)
	{
	case 0:
		tcList = BulletTCList<RedBullet3>();
		break;
	case 1:
		tcList = BulletTCList<BuleBullet3>();
		break;
	case 2:
		tcList = BulletTCList<GreenBullet3>();
		break;
	case 3:
		tcList = BulletTCList<YellowBullet3>();
		break;
	case 4:
		tcList = BulletTCList<PurpleBullet3>();
		break;
	case 5:
		tcList = BulletTCList<LightBlueBullet3>();
		break;
	}
	return tcList;
}
std::vector<Texcoord> RandomBulletColor(int type)
{
	std::vector<Texcoord> tcList;
	switch (type)
	{
	case 0:
		tcList = BulletTCList<RedBullet8>();
		break;
	case 1:
		tcList = BulletTCList<BuleBullet8>();
		break;
	case 2:
		tcList = BulletTCList<GreenBullet8>();
		break;
	case 3:
		tcList = BulletTCList<YellowBullet8>();
		break;
	case 4:
		tcList = BulletTCList<PurpleBullet8>();
		break;
	case 5:
		tcList = BulletTCList<LightBlueBullet8>();
		break;
	}
	return tcList;
}
std::vector<Texcoord> TwistBulletColor(int type)
{
	std::vector<Texcoord> tcList;
	switch (type)
	{
	case 0:
		tcList = BulletTCList<RedBullet2>();
		break;
	case 1:
		tcList = BulletTCList<BuleBullet2>();
		break;
	case 2:
		tcList = BulletTCList<GreenBullet2>();
		break;
	case 3:
		tcList = BulletTCList<YellowBullet2>();
		break;
	case 4:
		tcList = BulletTCList<PurpleBullet2>();
		break;
	case 5:
		tcList = BulletTCList<LightBlueBullet2>();
		break;
	}
	return tcList;
}
std::vector<Texcoord> StraightLineBulletColor(int type)
{
	std::vector<Texcoord> tcList;
	switch (type)
	{
	case 0:
		tcList = BulletTCList<RedBullet1>();
		break;
	case 1:
		tcList = BulletTCList<BuleBullet1>();
		break;
	case 2:
		tcList = BulletTCList<GreenBullet1>();
		break;
	case 3:
		tcList = BulletTCList<YellowBullet1>();
		break;
	case 4:
		tcList = BulletTCList<PurpleBullet1>();
		break;
	case 5:
		tcList = BulletTCList<LightBlueBullet1>();
		break;
	}
	return tcList;
}
