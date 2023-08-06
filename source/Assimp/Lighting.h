#pragma once
#include "../MySource/Shader/Shader.h"
#include "BufferObject/BufferObject.h"
#include <vector>
class Camera;

class LightBuffer;
using LightBufferPtr = std::shared_ptr<LightBuffer>;

struct DirectionLight {
	glm::vec3 direction{ 0.57735f, -0.57735f, -0.57735f };	//向き
	glm::vec3 color{ 1.f, 0.9f, 0.8f };				//環境色
	//glm::vec3 diffuse{ 0.5f, 0.5f, 0.5f };				//拡散色
	//glm::vec3 specular{ 1.0f, 1.0f, 1.0f };				//反射色
};

struct PointLight {
	glm::vec3 position{ 1.2f, 1.0f, 2.0f };				//位置
	glm::vec3 color{ 0.2f, 0.2f, 0.2f };				//環境
	//float constant{ 1.f };								//定数値
	//float linear{ 0.09f };								//線項  
	//float quadratic{ 0.032f };							//二次項
};

struct SpotLight {
	glm::vec3 direction{ -0.2f, -1.0f, -0.3f };			//向き
	glm::vec3 position{ 1.2f, 1.0f, 2.0f };				//位置
	glm::vec3 color{ 0.2f, 0.2f, 0.2f };				//環境
	float cutOff{ glm::cos(glm::radians(12.5f)) };		//内側の角度 spot
	float outerCutOff{ glm::cos(glm::radians(15.0f)) };	//外側の角度 spot	s	
	float constant{ 1.f };								//定数値 spot & point 
	float linear{ 0.09f };								//線項 spot & point
	float quadratic{ 0.032f };							//二次項 spot & point
};

class LightBuffer
{
public:
	static LightBufferPtr Create(size_t lightCount) {
		return std::make_shared<LightBuffer>(lightCount);
	}
	explicit LightBuffer(size_t lightCount);
	~LightBuffer() = default;

	LightBuffer(const LightBuffer&) = delete;
	LightBuffer& operator=(const LightBuffer&) = delete;

	//ライトデータ作成
	void BeginUpdate();

	void AddDirectionLight(DirectionLight& light);

	void AddPointLight(PointLight& light);

	void AddSpotLight(SpotLight& light);

	void EndUpdate();
	void SwapBuffers();

	void Bind();
	void Unbind();
	template <typename T>
	void MemoryCopy(MappedBufferObjectPtr& ssbo, std::vector<T>& buffer) {
		ssbo->WaitSync();
		uint8_t* p = ssbo->GetMappedAddress();
		const int lightCount[4] = { static_cast<int>(buffer.size()) };
		memcpy(p, lightCount, sizeof(lightCount));
		if (!buffer.empty()) {
			p += sizeof(lightCount);
			const size_t size = (std::min<size_t>)(
				ssbo->GetSize(), buffer.size() * sizeof(T));
			memcpy(p, buffer.data(), size);

			//bufferの最小化
			buffer.clear();
			buffer.shrink_to_fit();
		}
	}

	int type = 0;
private:
	DirectionLight dirLightBuffer = {};
	std::vector<PointLight> pointLightBuffer = {};
	std::vector<SpotLight> spotLightBuffer = {};

	MappedBufferObjectPtr dirSsbo;
	MappedBufferObjectPtr pointSsbo;
	MappedBufferObjectPtr spotSsbo;

	bool isUpdating = false;
};
