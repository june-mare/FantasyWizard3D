#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"

class ScaleRotation :public Component
{
	float angle;
	float scale = 1.0f;
	float sizeX = 1.0f, sizeY = 1.0f;	
	int count;
public:
	GameObjectPtr target;
	ScaleRotation(GameObjectPtr gameObject)
		:Component(gameObject) { name = "ScaleRotation"; }
	virtual void Start()override
	{
		isStart = true;
	}

	virtual void Update()override;
	
	virtual void Debug()override
	{
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::DragFloat("scale", &scale);
			ImGui::DragFloat("angle", &angle);
		}
	}
};
