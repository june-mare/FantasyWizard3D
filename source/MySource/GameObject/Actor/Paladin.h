#pragma once
#include "../GameObject.h"


class Paladin
	:public GameObject
{
private:

public:
	void ActorStart()noexcept override;
	void ActorUpdate()noexcept override;
};