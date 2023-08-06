#pragma once
#include "2DImage/Texture.h"
#include "Shader/Shader.h"
#include "BufferObject/VertexArray.h"

class LoadingGauge
{
	VertexArrayPtr vao;
	VertexArrayPtr gaugeVao;
	TexturePtr texGauge{};
	TexturePtr texGaugeBar{};
	glm::vec2 viewSize{ 1920.f,1080.f };
	ShaderPtr shader;
	glm::vec3 position{960.f,50.f,0.f};
	float percentage = 0.f;
public:
	LoadingGauge(VertexArrayPtr vao, VertexArrayPtr gaugevao,
		ShaderPtr shader,TexturePtr gauge,TexturePtr gaugeBar);

	inline void SetPercentage(float percent){
		percentage = percent;
	}
	
	void Draw();
};