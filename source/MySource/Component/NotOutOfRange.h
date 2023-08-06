#pragma once
#include "Component/Component.h"
#include "GameObject/GameObject.h"
#include <iostream>

/*画面外と判定されたオブジェクトを削除する*/
class NOutOfBounds :public Component
{
public:
	NOutOfBounds(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "OutOfScreen";
	};
	virtual ~NOutOfBounds() = default;

	virtual void Update()override;
	
};
