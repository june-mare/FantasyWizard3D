#pragma once
#include <glad/glad.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "../MySource/Shader/Shader.h"
#include "../MySource/BufferObject/VertexArray.h"

namespace Mesh
{
#define MAX_BONE_INFLUENCE 4

	struct Vertex
	{
		//頂点
		glm::vec3 position;
		//法線
		glm::vec3 normal;
		//テックスコード
		glm::vec2 texcoord;
		//回転
		glm::vec3 tangent;
		//ビット回転
		glm::vec3 bitTangents;
		//ボーン情報
		int boneIds[MAX_BONE_INFLUENCE];
		//ボーンをつなぐ比重
		float weights[MAX_BONE_INFLUENCE];
	};

	struct Material {
		glm::vec4 baseColor			 = glm::vec4(1.f);	//基本色
		float roughness				 = 1.f;				//表面の粗さ 粗1～滑0
		float metallic				 = 0.04f;				//金属性 0 = 非金属,1 = 金属			
		float shininess				 = 0.5f;			//鏡面反射の強さ
		float emission				 = 1.f;				//光の放射
	};

	struct Texture
	{
		enum Type {
			diffuse,
			normal,
			specular,
			height,
		};
		unsigned int id = 0;
		int typeId = 0;	
		std::string path{};
	};

	class Mesh
	{
	private:
		unsigned int ibo, vbo;
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		VertexArrayPtr vao;
		Material material;
	private:
		void SetupMesh();
	public:
		Mesh(
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<Texture> textures);
		virtual ~Mesh() = default;

		void Draw(Shader& shader, bool isInstance = false)const ;
		void Draw(Shader& shader,Material& material,bool isInstance = false)const ;

		bool operator==(const Mesh& mesh) {
			return mesh.vao == this->vao;
		}
		bool operator!=(const Mesh& mesh) {
			return mesh.vao != this->vao;
		}
		bool operator<(const Mesh& mesh) const{
			return this->vao < mesh.vao;
		}
	};
}