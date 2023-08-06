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
		//���_
		glm::vec3 position;
		//�@��
		glm::vec3 normal;
		//�e�b�N�X�R�[�h
		glm::vec2 texcoord;
		//��]
		glm::vec3 tangent;
		//�r�b�g��]
		glm::vec3 bitTangents;
		//�{�[�����
		int boneIds[MAX_BONE_INFLUENCE];
		//�{�[�����Ȃ���d
		float weights[MAX_BONE_INFLUENCE];
	};

	struct Material {
		glm::vec4 baseColor			 = glm::vec4(1.f);	//��{�F
		float roughness				 = 1.f;				//�\�ʂ̑e�� �e1�`��0
		float metallic				 = 0.04f;				//������ 0 = �����,1 = ����			
		float shininess				 = 0.5f;			//���ʔ��˂̋���
		float emission				 = 1.f;				//���̕���
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