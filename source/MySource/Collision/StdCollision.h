#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace COLLISION
{
	/**
	* �����s���E�{�b�N�X
	*/
	struct AABB
	{
		glm::vec3 min = { 0, 0, 0 };
		glm::vec3 max = { 0, 0, 0 };
	};


}