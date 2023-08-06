#pragma once
#include <glad/glad.h> 
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>
#include "Mesh.h"
#include "BoneInfo.h"
#include "GlmHelper.h"
#include "../MySource/Shader/Shader.h"



class Model
{
public:
	std::vector<Mesh::Texture> textures_loaded;
	Mesh::Material material;
	std::vector<Mesh::Mesh> meshes;
	std::string directory;
	bool gammaCorrection = false;
	std::string name = "";
	int id = 0;
private:
	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;

public:
	Model(const char* path,std::string name = "",float shining = 0.f);
	//mapのスタティックメッシュ用
	Model() = default;
	virtual ~Model() = default;
	void Draw(Shader& shader);
	auto& GetBoneInfoMap() { return boneInfoMap; }
	int& GetBoneCount() { return boneCounter; }
	void operator=(const Model& model) {
		meshes.insert(meshes.begin(),
			model.meshes.begin(),
			model.meshes.end());
		textures_loaded.insert(textures_loaded.begin(),
			model.textures_loaded.begin(),
			model.textures_loaded.end());
		name = model.name;
		directory = model.directory;
		gammaCorrection = model.gammaCorrection;
	}

private:
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh::Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Mesh::Texture> LoadMaterialTextures
	(aiMaterial* mat, aiTextureType type,Mesh::Texture::Type typeId);
	void SetVertexBoneDataToDefault(Mesh::Vertex& vertex);
	void SetVertexBoneData(Mesh::Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	unsigned int TextureFromFile
	(const char* path, const std::string& directory, bool gamma = false);
};
