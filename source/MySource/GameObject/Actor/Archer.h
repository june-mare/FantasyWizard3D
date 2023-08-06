#pragma once
#include "../GameObject.h"

class Archer
	:public GameObject
{
private:

public:
	void ActorStart()noexcept override;
};