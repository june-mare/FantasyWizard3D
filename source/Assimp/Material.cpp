#include "Material.h"
#include "Debug/Debug.h"

MaterialBuffer::MaterialBuffer(size_t materialCount)
{
	ssbo = MappedBufferObject::Create(
		materialCount * sizeof(Mesh::Material),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	//LOG("MaterialBufferを作成（最大マテリアル数= %d )", materialCount);
}

void MaterialBuffer::BeginUpdate()
{
	if (isUpdating)return;
	isUpdating = true;

	buffer.reserve(ssbo->GetSize() / sizeof(Mesh::Material));
}

int MaterialBuffer::AddMaterial(const Mesh::Material& material)
{
	if (!isUpdating)return 0;
	unsigned int id = static_cast<unsigned int>(buffer.size());
	buffer.push_back(material);
	return id;
}

void MaterialBuffer::EndUpdate()
{
	if (!isUpdating) return;

	isUpdating = false;

	ssbo->WaitSync();
	uint8_t* p = ssbo->GetMappedAddress();
	if (!buffer.empty()) {
		const size_t size = (std::min<size_t>)(
			ssbo->GetSize(), buffer.size() * sizeof(Mesh::Material));
		memcpy(p, buffer.data(), size);

		//bufferの最小化
		buffer.clear();
		buffer.shrink_to_fit();
	}
}

void MaterialBuffer::SwapBuffers()
{
	ssbo->SwapBuffers();
}

void MaterialBuffer::Bind()
{
	if (isUpdating) {
		LOG_WARNING("EndUpdateが呼ばれていません");
		EndUpdate();
	}
	size_t testData = ssbo->GetSize();
	ssbo->Bind(3, 0, ssbo->GetSize());
}

void MaterialBuffer::Unbind()
{
	glBindBufferRange(ssbo->GetType(), 3, 0, 0, 0);
}