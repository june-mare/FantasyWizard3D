#pragma once
#include "Renderer.h"
#include "Shader/Shader.h"
#include "Std3D/GLTFMesh.h"

/**
* glTFメッシュ描画コンポーネント
*/
class GltfMeshRenderer : public Renderer
{
public:
	GltfMeshRenderer() = default;
	virtual ~GltfMeshRenderer() = default;

	// 描画
	virtual void Draw(const Shader& shader,
		const glm::mat4& matGameObject,
		std::vector<StdMesh::MaterialPtr> materials
	)override
	{
		if (!file || meshIndex >= static_cast<int>(file->meshes.size())) {
			return; // ファイルまたはメッシュが存在しない場合は何もしない

		}

		// 座標変換行列を設定
		const glm::mat4 matModel = matGameObject * file->matRoot;
		glProgramUniformMatrix4fv(shader, 0, 1, GL_FALSE, &matModel[0][0]);

		// マテリアルが指定されていない場合、共有マテリアルを使って描画する
		std::vector<GLTFMesh::GltfMaterialPtr>
			pMaterials;
		

		if (materials.empty()) {
			
			
			//pMaterials = &file->materials;

		}

		// インデックスが負の場合は全メッシュを描画、0以上の場合は指定されたメッシュだけ描画
		if (meshIndex >= 0) {
			for (const auto& e : file->meshes) {
				//GLTFMesh::Draw(e, *pMaterials,shader);

			}

		}
		else {
			//GLTFMesh::Draw(file->meshes[meshIndex], *pMaterials, *shader);

		}
	}

	static const int drawAllMeshes = -1; // 全てのメッシュを描画するための番号

	GLTFMesh::GltfFilePtr file;                       // 描画に使うglTFファイル
	std::vector<GLTFMesh::GltfMaterialPtr> materials; // 固有マテリアル
	int meshIndex = drawAllMeshes;               // 描画するメッシュ番号
};
using GltfMeshRendererPtr = std::shared_ptr<GltfMeshRenderer>;