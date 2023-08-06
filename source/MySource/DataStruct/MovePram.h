#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Time/Time.h"

class MovePram 	
{
public:
	MovePram() = default;
	virtual ~MovePram() = default;
	
	float speed = 1.0f;
	float acc = 0.0f;
	float accIncrease = 0.2f;
	float accMax = 5.0f;
	glm::vec3 moveVector = { 0.1f,0.f,0.f };

	void Acceleration()noexcept
	{
		if (accMax > acc)acc += accIncrease;
		if (accMax < acc)acc = accMax;
	}
	void Reset()noexcept
	{
		acc = 0.0f;
	}
	glm::vec3 IncreasePos()noexcept
	{
		return glm::normalize(moveVector) * (acc + speed) * Time::deltaTime;
	}
};