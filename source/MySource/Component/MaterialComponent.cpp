#include "MaterialComponent.h"
#include "GameObject/ObjectManager.h"

void MaterialComponent::Update(){
	ObjectManager& manager = ObjectManager::Instance();
	gameObject->materialID = manager.AddMaterial(material);
}

void MaterialComponent::Debug(){
	ImGui::Text(name.c_str());
	ImGui::ColorPicker4("baseColor", &material.baseColor.x);
	ImGui::SliderFloat("roughness",  &material.roughness, 0.f, 1.f);
	ImGui::DragFloat("matallic",	 &material.metallic);
	ImGui::InputFloat("emission",	 &material.emission);
	ImGui::DragFloat("shininess",	 &material.shininess);
}
