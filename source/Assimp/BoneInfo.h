#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include<glm/glm.hpp>
#include<string>

struct BoneInfo
{
	int id;
	glm::mat4 offset;
	std::string name;
};