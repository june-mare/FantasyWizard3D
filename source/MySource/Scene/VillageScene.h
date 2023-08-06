#pragma once
#include "Scene.h"

class VillageScene 
	:public Scene
{
	float timer = 0.f;
	float interval = 0.5f;
	bool isSwich = false;
public:
	VillageScene() = default;
	virtual ~VillageScene() = default;
	virtual bool Initialize()override;
	virtual void Execute()override;
	virtual void Debug()override;
	virtual void SetUp()override;
};
