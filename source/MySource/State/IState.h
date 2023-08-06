#pragma once
#include <memory>

class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;

namespace STATE
{
	class IState
	{
	public:
		virtual void Initialize()noexcept = 0;
		virtual void Execute(float deltaTime)noexcept = 0;
		virtual void Finalize()noexcept = 0;
		virtual void giveGameObject(GameObjectPtr gameObject)noexcept = 0;
		virtual void Debug()noexcept = 0;
	};
}
