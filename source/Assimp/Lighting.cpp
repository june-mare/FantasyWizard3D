#include "Lighting.h"
#include "Debug/Debug.h"

LightBuffer::LightBuffer(size_t lightCount)
{
	dirSsbo = MappedBufferObject::Create(
		sizeof(DirectionLight),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	pointSsbo = MappedBufferObject::Create(
		lightCount * sizeof(PointLight),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	spotSsbo = MappedBufferObject::Create(
		lightCount * sizeof(SpotLight),
		GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//LOG("LightBuffer‚ðì¬iÅ‘åƒ‰ƒCƒg”= %d", lightCount);
}

void LightBuffer::BeginUpdate()
{
	if (isUpdating)return;
	isUpdating = true;
	pointLightBuffer.reserve(pointSsbo->GetSize() / sizeof(PointLight));
	spotLightBuffer.reserve(spotSsbo->GetSize() / sizeof(SpotLight));
}

void LightBuffer::AddDirectionLight(DirectionLight& light)
{
	if (!isUpdating)return;
	dirLightBuffer = light;
}

void LightBuffer::AddPointLight(PointLight& light)
{
	if (!isUpdating)return;
	pointLightBuffer.push_back(light);
}

void LightBuffer::AddSpotLight(SpotLight& light)
{
	if (!isUpdating)return;
	spotLightBuffer.push_back(light);
}

void LightBuffer::EndUpdate()
{
	if (!isUpdating) return;

	isUpdating = false;

	dirSsbo->WaitSync();
	uint8_t* p = dirSsbo->GetMappedAddress();
	memcpy(p, &dirLightBuffer, dirSsbo->GetSize());

	MemoryCopy<PointLight>(pointSsbo, pointLightBuffer);
	MemoryCopy<SpotLight>(spotSsbo, spotLightBuffer);
}

void LightBuffer::SwapBuffers()
{
	dirSsbo->SwapBuffers();
	pointSsbo->SwapBuffers();
	spotSsbo->SwapBuffers();
}

void LightBuffer::Bind()
{
	if (isUpdating) {
		LOG_WARNING("EndUpdate‚ªŒÄ‚Î‚ê‚Ä‚¢‚Ü‚¹‚ñ");
		EndUpdate();
	}
	dirSsbo->Bind(4, 0, dirSsbo->GetSize());
	pointSsbo->Bind(5, 0, pointSsbo->GetSize());
	spotSsbo->Bind(6, 0, spotSsbo->GetSize());
}

void LightBuffer::Unbind()
{
	glBindBufferRange(dirSsbo->GetType(), 4, 0, 0, 0);
	glBindBufferRange(pointSsbo->GetType(), 5, 0, 0, 0);
	glBindBufferRange(spotSsbo->GetType(), 6, 0, 0, 0);
}