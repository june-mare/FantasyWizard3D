#pragma once
#include "../GameObject.h"


class Beast
	:public GameObject
{
private:

public:
	void ActorStart()noexcept override;
};