#pragma once
#include "Time.h"

class Timer
{
	float increass = 0.f;
	float time = 1.f;
public:
	inline void Reset() {
		increass = 0.f;
	}
	inline bool TimeUp() {
		return increass >= time;
	}
	inline void Execute() {
		if (time > increass)increass += Time::deltaTime;
		if (TimeUp())increass = time;
	}
	inline void SetTimer(float time) {
		this->time = time;
	}
};