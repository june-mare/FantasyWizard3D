#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Std3D/StdMesh.h"
#include "Renderer.h"
#include "GameObject/Transform.h"

/**
* メッシュ描画コンポーネント
*/
class MeshRenderer
	: public Renderer
{
public:
	MeshRenderer(GameObjectPtr gameObject)
		:Renderer(gameObject) {
		name = "MeshRenderer";
	};
	virtual ~MeshRenderer() = default;

	// 描画
	virtual void Draw(const Shader& shader,
		const glm::mat4& matGameObject,
		std::vector<StdMesh::MaterialPtr> materials)override
	{
		if (!mesh)return;
		
		transform.computeModelMatrix(matGameObject);
		glm::mat4 model = transform.getLocalModelMatrix();
		
		shader.SetMat4("model", model);

		if (materials.empty()) {
			StdMesh::Draw(*mesh, mesh->materials, shader);
		}
		else {
			StdMesh::Draw(*mesh, materials, shader);
		}
	}

	void Debug()override{
		ImGui::Text(name.c_str());
		
		glm::vec3 pos = transform.getLocalPosition();
		ImGui::DragFloat3("position",&pos.x);

		glm::vec3 rotate = transform.getLocalRotation();
		ImGui::DragFloat3("rotate", &rotate.x);

		glm::vec3 scale = transform.getLocalScale();
		ImGui::DragFloat3("scale", &scale.x);
	}

	StdMesh::StaticMeshPtr mesh;
	Transform transform;
};
using MeshRendererPtr = std::shared_ptr<MeshRenderer>;