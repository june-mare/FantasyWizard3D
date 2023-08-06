#pragma once
#include <glad/glad.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <memory>

class UniformBufferObject;
using UniformBufferObjectPtr = std::shared_ptr<UniformBufferObject>;

class UniformBufferObject
{
private:
	unsigned int id = 0;
	size_t bufferSize = 0;
	int bindingPoint = 0;
public:
	static UniformBufferObjectPtr Create(size_t size,
		const void* data = nullptr, GLbitfield flags = 0);

	explicit UniformBufferObject(size_t size,
		const void* data = nullptr, GLbitfield flags = 0);
	~UniformBufferObject() {};

	void Bind();

	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }
};