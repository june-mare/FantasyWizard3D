#pragma once
#include "../GameObject.h"

class DefaultCube
	:public GameObject 
{
public:
	DefaultCube() = default;
	~DefaultCube() = default;
	
	void ActorStart()noexcept override;
};