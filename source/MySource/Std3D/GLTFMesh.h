#pragma once
#undef APIENTRY
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
class BufferObject;
using BufferObjectPtr = std::shared_ptr<BufferObject>;
class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;
class Shader;
using ShaderPtr = std::shared_ptr<Shader>;


namespace GLTFMesh {

	/**
	* glTF�̃}�e���A��
	*/
	struct GltfMaterial
	{
		std::string name = "";				// �}�e���A����
		glm::vec4 baseColor = glm::vec4(1); // ��{�F
		TexturePtr texDiffuse = nullptr;	// ��{�F�e�N�X�`��
		TexturePtr texNormal = nullptr;     // �@���e�N�X�`��
		TexturePtr texSpecular = nullptr;   // �@���e�N�X�`��
		float roughness = 1;				// �\�ʂ̑e���B�e��=1 �Ȃ߂炩=0
	};
	using GltfMaterialPtr = std::shared_ptr<GltfMaterial>;

	/**
	* �v���~�e�B�u�f�[�^
	*/
	struct GltfPrimitive
	{
		GLenum mode = GL_TRIANGLES;		// �v���~�e�B�u�̎��
		GLsizei count = 0;				// �`�悷��C���f�b�N�X��
		GLenum type = GL_UNSIGNED_SHORT;// �C���f�b�N�X�f�[�^�^
		const GLvoid* indices = 0;		// �`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g
		GLint baseVertex = 0;			// �C���f�b�N�X0�ԂƂ݂Ȃ���钸�_�z����̈ʒu

		VertexArrayPtr vao = nullptr;	// �v���~�e�B�u�pVAO
		size_t materialNo = 0;			// �}�e���A���ԍ�
	};

	/**
	* ���b�V���f�[�^
	*/
	struct GltfMesh
	{
		std::string name = "";						// ���b�V����
		std::vector<GltfPrimitive> primitives = {}; // �v���~�e�B�u�z��
	};

	/**
	* �t�@�C��
	*/
	struct GltfFile
	{
		std::string name = "";							// �t�@�C����
		std::vector<GltfMesh> meshes = {};				// ���b�V���z��
		std::vector<GltfMaterialPtr> materials = {};	// �}�e���A���z��
		glm::mat4 matRoot = glm::mat4(1);				// ��{�p���s��
	};
	using GltfFilePtr = std::shared_ptr<GltfFile>;

	void Draw(const GltfMesh& mesh,
		const std::vector<GltfMaterialPtr>&materials, Shader& shader);

	/**
	* glTF�t�@�C������ǂݍ��񂾃��b�V�����Ǘ�����N���X
	*/
	class GltfFileBuffer
	{
	public:
		// glTF�t�@�C���o�b�t�@���쐬����
		static std::shared_ptr<GltfFileBuffer> Create(size_t bufferCapacity) {
			return std::make_shared<GltfFileBuffer>(bufferCapacity);

		}

		// �R���X�g���N�^�E�f�X�g���N�^
		explicit GltfFileBuffer(size_t bufferCapacity);
		~GltfFileBuffer() = default;

		// �t�@�C���̓ǂݍ���
		GltfFilePtr Load(const char* filename);

	private:
		GltfFilePtr Parse(const char* text, const char* foldername);

		BufferObjectPtr buffer = nullptr;          // bin�t�@�C����ǂݍ��ރo�b�t�@
		uint8_t* pBuffer = nullptr;				   // �}�b�v���ꂽ�A�h���X
		GLsizeiptr curBufferSize = 0;              // �������ݍς݃f�[�^��
		GltfMaterialPtr defaultMaterial;           // �}�e���A�����ݒ莞�Ɏg�p����}�e���A��

		// �t�@�C���Ǘ��p�̘A�z�z��
		std::unordered_map<std::string, GltfFilePtr> files;

	};
	using GltfFileBufferPtr = std::shared_ptr<GltfFileBuffer>;



}//GLTFMesh