#pragma once

#include "Component.h"
#include "../GameObject/GameObject.h"

class ActiveTimer
	:public Component
{
	float timer = 0.f;
public:
	float activeTime = 1.0f;
	
	
	ActiveTimer(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "ActiveTimer";
	}
	virtual ~ActiveTimer() = default;

	void Update()
	{
		timer += Time::deltaTime;
		if (activeTime <= timer)
		{
			gameObject->IsDead(true);
		}
	}
};
