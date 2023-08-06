#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <iterator>
#include "Mesh.h"
#include "BufferObject/AnimBuffer.h"

enum class ModelType {
	mesh,
	anim,
};

class InstanceModelDataList
{	

	//�f�[�^�z��ɏ������邽�߂̉��z��
	std::vector<AnimData>
		tmpAnimDataList = {};

	std::vector<MeshData>
		tmpMeshDataList = {};

	//�ǂ����̃��X�g�ɓ���邩�̕ϐ�
	ModelType type = ModelType::mesh;
	
	//�O��ƈႤ�ꍇ�f�[�^�z��ɓ���邽�߂̕ϐ�
	Mesh::Mesh* beforMesh = nullptr;

public:
	//�C���X�^���X�`��p�̃f�[�^�z��
	std::map<Mesh::Mesh, std::vector<AnimData>>
		animDataList = {};

	std::map<Mesh::Mesh, std::vector<MeshData>>
		meshDataList = {};
	
	void SetDataList() {
		if (type == ModelType::mesh) {
			std::copy(tmpMeshDataList.begin(),
				tmpMeshDataList.end(),
				std::back_inserter(meshDataList[*beforMesh]));
			tmpMeshDataList.clear();
		}
		else {
			std::copy(tmpAnimDataList.begin(),
				tmpAnimDataList.end(),
				std::back_inserter(animDataList[*beforMesh]));
			tmpAnimDataList.clear();
		}

		delete beforMesh;
		beforMesh = nullptr;	
	}

	void SetAnimData(int materialID,const Mesh::Mesh& mesh,const glm::mat4& model,
		const std::vector<glm::mat4>& transforms) {
		if (beforMesh == nullptr)
			beforMesh = new Mesh::Mesh(mesh);
		if (*beforMesh != mesh) {
			SetDataList();
			beforMesh = new Mesh::Mesh(mesh);
		}
		if (tmpAnimDataList.empty())
			tmpAnimDataList.reserve(100);
		tmpAnimDataList.push_back(AnimDataSetter(materialID,model, transforms));
		type = ModelType::anim;
	}

	void SetMeshData(int materialID, const Mesh::Mesh& mesh,const glm::mat4& model) {
		if (beforMesh == nullptr) {
			beforMesh = new Mesh::Mesh(mesh);
		}
		if (*beforMesh != mesh) {
			SetDataList();
			beforMesh = new Mesh::Mesh(mesh);
		}

		if (tmpMeshDataList.empty())
			tmpMeshDataList.reserve(100);
		tmpMeshDataList.push_back(MeshDataSetter(materialID,model));
		type = ModelType::mesh;
	}

	void DataClear() {
		animDataList.clear();
		meshDataList.clear();
		tmpAnimDataList.clear();
		tmpAnimDataList.shrink_to_fit();
		tmpAnimDataList.reserve(100);
		tmpMeshDataList.clear();
		tmpMeshDataList.shrink_to_fit();
		tmpMeshDataList.reserve(100);
		delete beforMesh;
		beforMesh = nullptr;
	}

	InstanceModelDataList() = default;
	~InstanceModelDataList() {
		delete beforMesh;
		beforMesh = nullptr;
	}
};
