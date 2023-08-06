#pragma once
#include "../Component/Component.h"
#include "../GameObject/GameObject.h"

/*画面外と判定されたオブジェクトを削除する*/
class OutOfScreen :public Component
{
public:
	OutOfScreen(GameObjectPtr gameObject) 
		:Component(gameObject) { name = "OutOfScreen"; };
	virtual ~OutOfScreen() = default;

	virtual void Update()override;

};