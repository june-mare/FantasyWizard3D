#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/transform.hpp>
#include "Time/Time.h"

class Lerp
{
public:
	glm::vec3 start = {};
	glm::vec3 end = {};
	float duration = 2.f;
	float elapsed = 0.f;

	inline bool EndCheck()noexcept {
		return elapsed < duration;
	}

	glm::vec3 Update()noexcept {
		float t = elapsed / duration;
		if (EndCheck())
			elapsed += Time::deltaTime;
		return glm::lerp(start, end, t);
	}
};

class Slerp
{
public:
	enum Type {
		xVector,
		yVector,
		zVector,
	};
	glm::quat start = {};
	glm::quat end = {};
	glm::vec3 startPosition = {}; 
	glm::vec3 startDirection = {};
	glm::vec3 endPosition = {};
	float duration = 2.0f;
	float elapsed = 0.0f;
	

	void SetUp(glm::vec3 start, glm::vec3 end,Type type) {
		startPosition = start; endPosition = end;
		glm::vec3 directionVector = glm::normalize(end - start);
		switch (type)
		{
		case Slerp::xVector:
			startDirection = glm::vec3(1.f, 0.f, 0.f);
			break;
		case Slerp::yVector:
			startDirection = glm::vec3(0.f, 1.f, 0.f);
			break;
		case Slerp::zVector:
			startDirection = glm::vec3(0.f, 0.f, 1.f);
			break;
		}

		this->start = glm::quatLookAt(directionVector, startDirection);
		this->end = glm::quat(startDirection, directionVector);
	}

	void SetUp(glm::vec3 start, glm::vec3 end, glm::vec3 axis) {
		startPosition = start; endPosition = end;
		startDirection = axis;
		glm::vec3 directionVector = glm::normalize(end - start);
	
		this->start = glm::quatLookAt(directionVector, startDirection);
		this->end = glm::quat(startDirection, directionVector);
	}

	inline bool EndCheck() noexcept {
		return elapsed < duration;
	}

	glm::vec3 Update() noexcept {
		float t = elapsed / duration;
		if (EndCheck())
			elapsed += Time::deltaTime;

		return startPosition + 
			(glm::normalize(glm::slerp(start, end, t)
			* startDirection
			* glm::length(endPosition - startPosition)));
	}
};

class Ease
{
public:
	enum Type {
		linearEasing,
		easeIn,
		easeOut,
		easeInOut,
	};

	glm::vec3 start = {};
	glm::vec3 end = {};
	Type type = linearEasing;
	float duration = 2.f;
	float elapsed = 0.f;
	
	inline bool EndCheck()noexcept {
		return elapsed < duration;
	}

	glm::vec3 Update()noexcept {
		float t = elapsed / duration;
		float easeT = 0.f;
		switch (type)
		{
		case Ease::linearEasing:
			easeT = t;
			break;
		case Ease::easeIn:
			easeT = t * t;
			break;
		case Ease::easeOut:
			easeT = t * (2 - t);
			break;
		case Ease::easeInOut:
			easeT = t < 0.5f ?
				2 * t * t :
				-1 + (4 - 2 * t) * t;
			break;		
		}
		
		if (EndCheck())
			elapsed += Time::deltaTime;
		return glm::lerp(start, end, easeT);
	}
};