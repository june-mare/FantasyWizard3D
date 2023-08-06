#pragma once
#include "Scene.h"
#include <memory>

class FadeController;

class TitleScene :public Scene
{
	bool fadein = false;
	float interval = 2;
	float allInterval = 10;
public:
	std::shared_ptr<FadeController> fades[13];
	TitleScene() = default;
	virtual ~TitleScene() = default;
	virtual bool Initialize()override;
	virtual void Execute()override;
};
