#pragma once
#include "../GameObject.h"
#include "DataStruct/AnimUseList.h"

class VillageMan
	:public GameObject
{
private:
	int type = 0;
public:
	VillageMan(int type = 0)
		:GameObject(),type(type){}
	void ActorStart()noexcept override;
	void Update()noexcept override;
};