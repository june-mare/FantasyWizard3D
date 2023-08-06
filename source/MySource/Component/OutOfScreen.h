#pragma once
#include "../Component/Component.h"
#include "../GameObject/GameObject.h"

/*��ʊO�Ɣ��肳�ꂽ�I�u�W�F�N�g���폜����*/
class OutOfScreen :public Component
{
public:
	OutOfScreen(GameObjectPtr gameObject) 
		:Component(gameObject) { name = "OutOfScreen"; };
	virtual ~OutOfScreen() = default;

	virtual void Update()override;

};