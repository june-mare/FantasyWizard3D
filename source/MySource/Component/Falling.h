#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"

const float GRAVITY_ACCELERATION = 0.98f;

class Falling
	:public Component
{
	float oldY = 0.f;
public:
	Falling(GameObjectPtr gameObject) :Component(gameObject) {
		name = "Falling";
		//oldY = gameObject->transform.getLocalPosition().y;
	}
	~Falling() = default;

	void Update()override {
		//const float nowY = gameObject->transform.getLocalPosition().y;
		glm::vec3 pos{};
		//if (std::abs(oldY - nowY) < 0.03f)
			//pos.y = 0.f;
		//else
		pos.y -= (mass * GRAVITY_ACCELERATION) * Time::deltaTime;
		//oldY = nowY;
		gameObject->AddPosition(pos);
	}

	void OnCollision(GameObject& other, std::vector<Collider::Tag> tagList)override {
		oldY = gameObject->transform.getLocalPosition().y;
		isFall = false;
	}

	void OutCollision()override {
		isFall = true;
	}

	bool isFall = true;
	float mass = 1.f;
};