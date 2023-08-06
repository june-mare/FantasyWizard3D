#pragma once
#include <glad/glad.h>
#include <memory>


// ��s�錾
class BufferObject;
using BufferObjectPtr = std::shared_ptr<BufferObject>;
class MappedBufferObject;
using MappedBufferObjectPtr = std::shared_ptr<MappedBufferObject>;

/**
* �o�b�t�@�I�u�W�F�N�g
*/
class BufferObject
{
public:
	static BufferObjectPtr Create(GLsizeiptr size,
		const void* data = nullptr, GLbitfield flags = 0);

	explicit BufferObject(GLsizeiptr size,
		const void* data = nullptr, GLbitfield flags = 0);
	~BufferObject();

	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }

private:
	GLuint id = 0;
	GLsizeiptr bufferSize = 0;
};

/**
* �o�b�t�@�I�u�W�F�N�g(�}�b�v����)
*/
class MappedBufferObject
{
public:
	static MappedBufferObjectPtr Create(
		GLsizeiptr size, GLenum type, GLenum access);

	MappedBufferObject(
		GLsizeiptr size, GLenum type, GLenum access);
	~MappedBufferObject();

	operator GLuint() const { return id; }
	GLsizeiptr GetSize() const { return bufferSize; }

	void WaitSync();
	uint8_t* GetMappedAddress() const;
	void Bind(GLuint index, size_t offset, size_t size);
	void SwapBuffers();

	GLenum GetType() const { return type; }

private:
	GLuint id = 0;
	GLsizeiptr bufferSize = 0;

	uint8_t* p = nullptr;          // �f�[�^�R�s�[��A�h���X
	size_t bufferIndex = 0;        // �R�s�[��o�b�t�@�̃C���f�b�N�X
	GLsync syncList[2] = { 0, 0 }; // �����I�u�W�F�N�g
	GLenum type = GL_ARRAY_BUFFER; // �o�b�t�@�̎��
};
