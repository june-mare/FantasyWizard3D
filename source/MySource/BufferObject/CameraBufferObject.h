#pragma once
#include <glad/glad.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <memory>

class CameraBufferObject;
using CameraBufferObjectPtr = std::shared_ptr<CameraBufferObject>;

class CameraBufferObject
{
	unsigned int id = 0;
	size_t bufferSize = 0;
public:
	static CameraBufferObjectPtr Create(GLsizeiptr size);
	CameraBufferObject(GLsizeiptr size);
	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }

	//全体のシェーダに使えるように設定する
	void SetBindBuffer(glm::mat4& projection, glm::mat4& view);
};