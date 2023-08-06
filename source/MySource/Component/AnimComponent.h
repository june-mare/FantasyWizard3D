#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"
#include "DataStruct/AnimUseList.h"


class AnimComponent 
	:public Component {
public:
	AnimComponent(GameObjectPtr gameObject) :
		Component(gameObject) {}
	virtual ~AnimComponent() = default;

	AnimUseList animUseList = {};
	bool exitTime = false;

};