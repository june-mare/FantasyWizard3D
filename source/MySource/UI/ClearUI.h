#pragma once
#include <memory>

class FadeController;
class Engine;

class ClearUI
{
public:
	ClearUI() = default;
	virtual ~ClearUI() = default;
	
	void Initialize(Engine& engine)noexcept;
	void Event(float deltaTime)noexcept;
	inline void setFadeFlg(const bool value)noexcept
	{
		fadein = value;
	}
private:
	std::shared_ptr<FadeController> fades[9]{};
	float interval	{2.0f};
	float allTime	{0.0f};
	bool fadein		{false};	
};

