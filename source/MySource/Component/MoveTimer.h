#pragma once
#ifndef MOVETIMER_H_INCLUDE
#define MOVETIMER_H_INCLUDE
#include "../GameObject/GameObject.h"
#include "../Component/Component.h"

class MoveTimer
	:public Component
{
	bool stopFlg = false;
	bool fadeOut = false;
	float movePoint = 0;
	float startPos = 0;
public:
	bool trriger = false;
	float interval = 3;
	float moveSpeed = 300;

	MoveTimer(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "MoveTimer";
	}
	virtual ~MoveTimer() = default;

	virtual void Start()override;
	

	virtual void CustomEvent()override;
	
};

#endif // !MOVETIMER_H_INCLUDE