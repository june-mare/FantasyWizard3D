#pragma once
#include <memory>
#include "../State/IState.h"
#include "../State/MapOneBossState.h"
#include <string>


class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;

class FactoryBase 
	:public std::enable_shared_from_this<FactoryBase>
{
protected:
	std::shared_ptr<STATE::IState> state;
	std::string name;	
	GameObjectPtr gameObject;
	bool debugCheck = false;
public:
	FactoryBase(GameObjectPtr _gameObject)
		noexcept:gameObject(_gameObject){}
	virtual ~FactoryBase()noexcept = default;
	
	template<typename T>
	void setState()noexcept
	{
		state.reset();
		state = std::make_shared<T>();
		state->giveBaseFactory(shared_from_this());
		state->giveGameObject(gameObject);
		state->Initialize();
	}
	
	virtual void Initialize()noexcept;

	virtual void Execute(float deltaTime)noexcept;
	
	virtual void Finalize()noexcept;

	virtual void Debug()noexcept;
	
	void StateExecute(float deltaTime)noexcept;	

	void DebugAll()noexcept;
};

