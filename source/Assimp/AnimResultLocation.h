#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "Time/Time.h"
#include "Debug/Debug.h"

class AnimResultLocation
{
	std::vector<glm::mat4> locationList = {};
	int size = 0;
	float duration = 0.f;
	float increas = 0.f;
	float ticksecond = 0.f;
public:
	void PushLocation(glm::mat4 location) {
		locationList.push_back(glm::mat4(location));
		size++;
	}
	inline void SetDuration(float time) {
		duration = time;
	}
	inline void SetIncreas(float time) {
		increas = time;
	}
	inline void SetTickTime(float time) {
		ticksecond = time;
	}
	glm::mat4& GetLocation(float count){	

		int itr = static_cast<int>(count / increas);
		if (itr >= size)itr = size - 1;
		
		return locationList[itr];				
	}

	inline int GetSize() {
		return size;
	}

	inline void ReserveList(int _size) {
		locationList.reserve(_size);
	}
};
