#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"


struct AnimData {
	int materialID[4] = { 0 };
	glm::mat4 model = {};
	glm::mat4 finalBoneMatrix[82] = {};
};

inline AnimData AnimDataSetter(int materialID, const glm::mat4& model,
	const std::vector<glm::mat4>& finalBoneMatrix) {
	AnimData r = {};
	r.materialID[0] = materialID;
	r.model = model;
	for (int size = 0; size < finalBoneMatrix.size(); size++) {
		r.finalBoneMatrix[size] = finalBoneMatrix[size];
	}
	return r;
}

struct MeshData {
	int materialID[4] = { 0 };
	glm::mat4 model = {};
};

inline MeshData MeshDataSetter(int materialID,const glm::mat4& model) {
	MeshData r = {};
	r.materialID[0] = materialID;
	r.model = model;
	return r;
}