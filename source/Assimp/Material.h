#pragma once
#include "BufferObject/BufferObject.h"
#include <vector>
#include "Mesh.h"

class MaterialBuffer;
using MaterialBufferPtr = std::shared_ptr<MaterialBuffer>;

class MaterialBuffer
{
private:
	MappedBufferObjectPtr ssbo;
	std::vector<Mesh::Material> buffer;
	bool isUpdating = false;
public:
	static MaterialBufferPtr Create(size_t materialCount) {
		return std::make_shared<MaterialBuffer>(materialCount);
	}
	explicit MaterialBuffer(size_t materialCount);
	~MaterialBuffer() = default;

	void BeginUpdate();
	
	int AddMaterial(const Mesh::Material& material);

	void EndUpdate();
	void SwapBuffers();

	void Bind();
	void Unbind();
};