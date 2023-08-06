#pragma once
#include <glad/glad.h>
#include <memory>


// 先行宣言
class BufferObject;
using BufferObjectPtr = std::shared_ptr<BufferObject>;
class MappedBufferObject;
using MappedBufferObjectPtr = std::shared_ptr<MappedBufferObject>;

/**
* バッファオブジェクト
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
* バッファオブジェクト(マップあり)
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

	uint8_t* p = nullptr;          // データコピー先アドレス
	size_t bufferIndex = 0;        // コピー先バッファのインデックス
	GLsync syncList[2] = { 0, 0 }; // 同期オブジェクト
	GLenum type = GL_ARRAY_BUFFER; // バッファの種類
};
