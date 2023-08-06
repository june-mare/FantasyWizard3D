#include <iostream>
#include <fstream>
#include <sstream>
#include "Model.h"
#include "Debug/Debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stbi/stb_image.h"

Model::Model(const char* path, std::string name, float shining)
	:name(name)
{
	LoadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_FlipUVs
		| aiProcess_CalcTangentSpace
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG_ERROR("ERROR::ASSIMP:: %s", importer.GetErrorString());
		return;
	}
	//LOG("%s を読み込みました", path.c_str());

	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));
	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);
}


void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh::Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Mesh::Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex vertex;
		SetVertexBoneDataToDefault(vertex);

		vertex.position = AssimpGLMHelper::GetGLMVec(mesh->mVertices[i]);

		//ノーマルがあるかどうか調べる
		if (mesh->HasNormals())
		{
			vertex.normal = AssimpGLMHelper::GetGLMVec(mesh->mNormals[i]);
		}

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texcoord = vec;
		}
		else
			vertex.texcoord = glm::vec2(0.0f, 0.0f);

		if (mesh->mTangents != NULL) {
			vertex.tangent = AssimpGLMHelper::GetGLMVec(mesh->mTangents[i]);
		}

		if (mesh->mBitangents != NULL) {
			vertex.bitTangents = AssimpGLMHelper::GetGLMVec(mesh->mBitangents[i]);
		}

		vertices.push_back(vertex);
	}

	//メッシュの各面（面はメッシュの三角形）を調べて、対応する頂点インデックスを取得します
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//LOG("vertices : %d \n indices : %d", vertices.size(), indices.size());

	//マテリアル処理
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//シェーダでのサンプラー名の規則を想定している。各拡散テクスチャの名前を付ける必要があり
	//texture_deffuseNとしてNは１からMAX_SAMPLER_NUMBER迄の連番です
	//次のリストに要約されているように同じことが他のテクスチャにも当てはまります
	//texture_diffuseN texture_specularN texture normalN

	//1 texture_diffuse maps
	std::vector<Mesh::Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, Mesh::Texture::diffuse);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//2 texture_samplar maps
	std::vector<Mesh::Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, Mesh::Texture::specular);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//3 texture_normal maps
	std::vector<Mesh::Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, Mesh::Texture::normal);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	//4 texture_height
	std::vector<Mesh::Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, Mesh::Texture::height);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return Mesh::Mesh(vertices, indices, textures);

}

std::vector<Mesh::Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type,Mesh::Texture::Type typeId)
{
	std::vector<Mesh::Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str = {};
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Mesh::Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.typeId = typeId;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return textures;
}

void Model::SetVertexBoneDataToDefault(Mesh::Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIds[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Mesh::Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.boneIds[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.boneIds[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	auto& boneInfoMap = this->boneInfoMap;
	int& boneCount = this->boneCounter;

	for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = AssimpGLMHelper::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	auto data = stbi_load(filename.c_str(), &width
		, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format = {};
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path; " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
