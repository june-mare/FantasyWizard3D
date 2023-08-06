#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "Component.h"
#include "GameObject/GameObject.h"
#include "Std3D/StdMesh.h"
#include "Std3D/GLTFMesh.h"


//先行宣言
class Shader;

/**
* レンダラーコンポーネントの基底クラス
*/
class Renderer : public Component
{
public:
	Renderer(GameObjectPtr gameObject) 
		:Component(gameObject) {
		name = "Renderer";
	}
	~Renderer() = default;

	virtual void Draw(const Shader& shader,
		const glm ::mat4& matGameObject
		, std::vector<StdMesh::MaterialPtr> materials = {}) {}
};
using RendererPtr = std::shared_ptr<Renderer>;