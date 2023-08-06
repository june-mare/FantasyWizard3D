#pragma once
#include "../FactoryBase.h"
#include "../../GameObject/GameObject.h"
#include "../../Component/ComponentInclude.h"


class Boss1Actor :
	public FactoryBase
{
	std::shared_ptr<Health> health;
	MovePram move;
	GameObjectPtr target;
	bool isHerf = false;
public:

	Boss1Actor(GameObjectPtr gameObject)
		:FactoryBase(gameObject){}
	virtual ~Boss1Actor()noexcept = default;

	void Initialize()noexcept override;
	void Execute(float deltaTime)noexcept override;
	void SummonObject()noexcept;
};