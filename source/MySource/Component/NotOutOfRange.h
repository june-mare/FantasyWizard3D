#pragma once
#include "Component/Component.h"
#include "GameObject/GameObject.h"
#include <iostream>

/*��ʊO�Ɣ��肳�ꂽ�I�u�W�F�N�g���폜����*/
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
