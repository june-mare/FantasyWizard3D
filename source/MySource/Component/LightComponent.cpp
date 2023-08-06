#include "LightComponent.h"
#include "GameObject/ObjectManager.h"

void LightComponent::Update() {
	ObjectManager& manager = ObjectManager::Instance();

	if (type == 0) {	
		DirectionLight data = dirlight;
		data.color *= intensity;
		manager.AddLight(data);
	}
	else if (type == 1) {
		pointlight.position = gameObject->transform.getGlobalPosition();
		PointLight data = pointlight;
		data.color *= intensity;
		manager.AddLight(data);
	}
	else if(type == 2){
		//spotlight.position = gameObject->transform.getGlobalPosition();
		SpotLight data = spotlight;
		data.color *= intensity;
		manager.AddLight(data);
	}

}

void LightComponent::Debug()
{
	if (ImGui::CollapsingHeader(name.c_str())) {
		
		ImGui::DragFloat("intensity", &intensity);
		
		if (type == 0) {

			ImGui::DragFloat3("direction", &dirlight.direction[0]);
			
			ImGui::DragFloat3("ambient", &dirlight.color[0]);
			//ImGui::DragFloat3("diffuse", &dirlight.diffuse[0]);
			//ImGui::DragFloat3("specular",&dirlight.specular[0]);
		}
		else if (type == 1) {
			ImGui::DragFloat3("ambient",	&pointlight.color[0]);
			//ImGui::DragFloat("constant",	&pointlight.constant);
			//ImGui::DragFloat("linear",		&pointlight.linear);
			//ImGui::DragFloat("quadratic",	&pointlight.quadratic);	
		}
		else if (type == 2) {
			ImGui::DragFloat3("direction",	&spotlight.direction[0]);
			ImGui::DragFloat3("ambient",	&spotlight.color[0]);
			ImGui::DragFloat("constant",	&spotlight.constant);
			ImGui::DragFloat("linear",		&spotlight.linear);
			ImGui::DragFloat("quadratic",	&spotlight.quadratic);
			ImGui::DragFloat("cutOff",		&spotlight.cutOff);
			ImGui::DragFloat("outerCutOff", &spotlight.outerCutOff);
		}
		ImGui::InputInt("type", &type);
	}
}
