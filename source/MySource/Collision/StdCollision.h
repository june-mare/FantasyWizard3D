#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace COLLISION
{
	/**
	* 軸平行境界ボックス
	*/
	struct AABB
	{
		glm::vec3 min = { 0, 0, 0 };
		glm::vec3 max = { 0, 0, 0 };
	};


}