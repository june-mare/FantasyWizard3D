#pragma once
#include "Renderer.h"
#include "Shader/Shader.h"
#include "Std3D/GLTFMesh.h"

/**
* glTF���b�V���`��R���|�[�l���g
*/
class GltfMeshRenderer : public Renderer
{
public:
	GltfMeshRenderer() = default;
	virtual ~GltfMeshRenderer() = default;

	// �`��
	virtual void Draw(const Shader& shader,
		const glm::mat4& matGameObject,
		std::vector<StdMesh::MaterialPtr> materials
	)override
	{
		if (!file || meshIndex >= static_cast<int>(file->meshes.size())) {
			return; // �t�@�C���܂��̓��b�V�������݂��Ȃ��ꍇ�͉������Ȃ�

		}

		// ���W�ϊ��s���ݒ�
		const glm::mat4 matModel = matGameObject * file->matRoot;
		glProgramUniformMatrix4fv(shader, 0, 1, GL_FALSE, &matModel[0][0]);

		// �}�e���A�����w�肳��Ă��Ȃ��ꍇ�A���L�}�e���A�����g���ĕ`�悷��
		std::vector<GLTFMesh::GltfMaterialPtr>
			pMaterials;
		

		if (materials.empty()) {
			
			
			//pMaterials = &file->materials;

		}

		// �C���f�b�N�X�����̏ꍇ�͑S���b�V����`��A0�ȏ�̏ꍇ�͎w�肳�ꂽ���b�V�������`��
		if (meshIndex >= 0) {
			for (const auto& e : file->meshes) {
				//GLTFMesh::Draw(e, *pMaterials,shader);

			}

		}
		else {
			//GLTFMesh::Draw(file->meshes[meshIndex], *pMaterials, *shader);

		}
	}

	static const int drawAllMeshes = -1; // �S�Ẵ��b�V����`�悷�邽�߂̔ԍ�

	GLTFMesh::GltfFilePtr file;                       // �`��Ɏg��glTF�t�@�C��
	std::vector<GLTFMesh::GltfMaterialPtr> materials; // �ŗL�}�e���A��
	int meshIndex = drawAllMeshes;               // �`�悷�郁�b�V���ԍ�
};
using GltfMeshRendererPtr = std::shared_ptr<GltfMeshRenderer>;