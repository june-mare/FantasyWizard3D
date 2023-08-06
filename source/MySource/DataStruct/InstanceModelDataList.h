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

	//データ配列に情報を入れるための仮配列
	std::vector<AnimData>
		tmpAnimDataList = {};

	std::vector<MeshData>
		tmpMeshDataList = {};

	//どっちのリストに入れるかの変数
	ModelType type = ModelType::mesh;
	
	//前回と違う場合データ配列に入れるための変数
	Mesh::Mesh* beforMesh = nullptr;

public:
	//インスタンス描画用のデータ配列
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
