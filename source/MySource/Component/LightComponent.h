#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"
#include "Lighting.h"


class LightComponent
	:public Component {
private:
	
public:
	LightComponent(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "LightComponent";
	}
	~LightComponent() = default;

	virtual void Update()override;
	virtual void Debug()override;

	DirectionLight dirlight = {};
	PointLight pointlight = {};
	SpotLight spotlight = {};
	float intensity = 1.f;
	int type = 0;
};