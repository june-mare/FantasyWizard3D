#pragma once
#include "BufferObject/VertexArray.h"
#include "BufferObject/BufferObject.h"
#include "Shader/Shader.h"
#include <array>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

class VisualCollision
{
	VertexArrayPtr vao{};
	BufferObjectPtr vbo{};
	BufferObjectPtr ibo{};
	size_t indicesSize = 0;
public:
	void SetUp(std::array<glm::vec3,8> vertices, std::array<unsigned int, 36> indices);

	void Draw(ShaderPtr& shader,const glm::mat4& model, glm::vec4 color);
};