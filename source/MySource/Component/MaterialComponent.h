#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"
#include "Mesh.h"


class MaterialComponent
	:public Component
{	
public:
	MaterialComponent(GameObjectPtr gameObject):
		Component(gameObject) {
		name = "MaterialComponent";
	}
	~MaterialComponent() = default;

	Mesh::Material material;
	void Update()override;
	void Debug()override;
};