#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"
#include "System/InputSystem.h"
#include "DataStruct/MovePram.h"
#include "Time/Timer.h"

class PlayerController :
	public Component {
	bool warking = false;
	bool isAttack = false;
	Timer walkTimer{};
	Timer attackTimer{};
	std::vector<AnimationPtr> animSetList{};
	int comboCount = 0;
public:
	MovePram move = {};
	std::shared_ptr<AnimUseList> animUseList = nullptr;
	PlayerController(GameObjectPtr gameObject) :Component(gameObject) {
		name = "PlayerController";
	}

	void Start()noexcept override;

	void Update()noexcept override;

	void CollideCreate(glm::vec3 point);
};