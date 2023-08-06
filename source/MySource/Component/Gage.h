#pragma once
#ifndef GAGE_H_INCLUDE
#define GAGE_H_INCLUDE
#include "../GameObject/GameObject.h"
#include "../2DImage/Sprite.h"
#include "../Component/Component.h"

class Gage :public Component
{
public:
	float sizex = 0;
	float baseData = 0;
	float startPos;
	float stopPos = 0;
	float movePram = 0;
	Gage(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "gage";
	}
	virtual ~Gage() = default;

	virtual void Start()override;
	
	virtual void Update()override;
	
};

#endif // !GAGE_H_INCLUDE