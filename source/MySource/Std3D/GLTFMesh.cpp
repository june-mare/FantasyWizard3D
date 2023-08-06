#include "GLTFMesh.h"
#include "GltfMesh.h"
#include "2DImage/Texture.h"
#include "BufferObject/BufferObject.h"
#include "BufferObject/VertexArray.h"
#include "Debug/Debug.h"
#include "Shader/Shader.h"
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include "../nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

namespace GLTFMesh {

	namespace /* unnamed */ {

		/**
		* �o�C�i���f�[�^
		*/
		struct BinaryData
		{
			GLsizeiptr offset;     // GPU��������̃f�[�^�J�n�I�t�Z�b�g
			std::vector<char> bin; // CPU�������ɓǂݍ��񂾃f�[�^
		};
		using BinaryList = std::vector<BinaryData>;

		/**
		* �t�@�C����ǂݍ���
		*
		* @param filename �ǂݍ��ރt�@�C����
		*
		* @return �ǂݍ��񂾃f�[�^�z��
		*/
		std::vector<char> ReadFile(const char* filename)
		{
			std::ifstream file(filename, std::ios::binary);
			if (!file) {
				LOG_WARNING("%s���J���܂���", filename);
				return {};

			}
			std::vector<char> buf(std::filesystem::file_size(filename));
			file.read(buf.data(), buf.size());
			return buf;
		}

		/**
		* ���_�A�g���r���[�g�ԍ�
		*/
		enum class AttribIndex
		{
			position,
			texcoord0,
			normal,
			tangent,
		};

		/**
		* �K�v�Ȓ��_�f�[�^�v�f���v���~�e�B�u�ɑ��݂��Ȃ��ꍇ�Ɏg���f�[�^
		*/
		struct DefaultVertexData
		{
			glm::vec3 position = glm::vec3(0);
			glm::vec2 texcoord0 = glm::vec2(0);
			glm::vec3 normal = glm::vec3(0, 0, -1);
			glm::vec4 tangent = glm::vec4(1, 0, 0, 1);
		};


		/**
		* �����̐����擾����
		*
		* @param accessor �p�����[�^�����A�N�Z�T
		*
		* @return �����̐�
		*/
		int GetComponentCount(const json& accessor)
		{
			// �^���Ɛ������̑Ή��\
			static const struct {
				const char* type;   // �^�̖��O
				int componentCount; // ������

			} componentCountList[] = {
				{ "SCALAR", 1 },
				{ "VEC2", 2 }, { "VEC3", 3 }, { "VEC4", 4 },
				{ "MAT2", 4 }, { "MAT3", 9 }, { "MAT4", 16 },
			};

			// �Ή��\���琬�������擾
			const std::string& type = accessor["type"].get<std::string>();
			for (const auto& e : componentCountList) {
				if (type == e.type) {
					return e.componentCount;

				}

			}
			LOG_WARNING("�s���Ȍ^`%s`���w�肳��Ă��܂�", type.c_str());
			return 1; // glTF�̎d�l�ǂ���Ȃ�A�����ɗ��邱�Ƃ͂Ȃ��͂�
		}

		/**
		* �X�g���C�h(���̗v�f�܂ł̋���)���擾����
		*
		* @param accessor   �A�N�Z�T
		* @param bufferView �o�b�t�@�r���[
		*
		* @return �X�g���C�h
		*/
		GLsizei GetByteStride(const json& accessor, const json& bufferView)
		{
			// byteStride����`����Ă�����A���̒l��Ԃ�
			const auto byteStride = bufferView.find("byteStride");
			if (byteStride != bufferView.end()) {
				return byteStride->get<int>();
			}

			// byteStride������`�̏ꍇ�A�v�f1���̃T�C�Y���X�g���C�h�Ƃ���
			// �Q�l: glTF-2.0�d�l 3.6.2.4. Data Alignment

			int componentSize = 1; // �����^�̃T�C�Y
			const int componentType = accessor["componentType"].get<int>();
			switch (componentType) {
			case GL_BYTE:           componentSize = 1; break;
			case GL_UNSIGNED_BYTE:  componentSize = 1; break;
			case GL_SHORT:          componentSize = 2; break;
			case GL_UNSIGNED_SHORT: componentSize = 2; break;
			case GL_UNSIGNED_INT:   componentSize = 4; break;
			case GL_FLOAT:          componentSize = 4; break;
			default:
				LOG_WARNING("glTF�̎d�l�ɂȂ��^%d���g���Ă��܂�", componentType);
				break;

			}

			// �X�g���C�h = �����^�̃T�C�Y * ������
			const int componentCount = GetComponentCount(accessor);
			return componentSize * componentCount;
		}

		/**
		* �f�[�^�̊J�n�ʒu���擾����
		*
		* @param accessor   �A�N�Z�T
		* @param bufferView �o�b�t�@�r���[
		* @param binaryList �o�C�i���f�[�^�z��
		*
		* @return �f�[�^�̊J�n�ʒu
		*/
		GLsizeiptr GetBinaryDataOffset(const json& accessor,
			const json& bufferView, const BinaryList& binaryList)
		{
			const int bufferId = bufferView["buffer"].get<int>();
			const int byteOffset = accessor.value("byteOffset", 0);
			const int baseByteOffset = bufferView.value("byteOffset", 0);
			return binaryList[bufferId].offset + baseByteOffset + byteOffset;
		}

		/**
		* ���_�A�g���r���[�g��ݒ肷��
		*
		* @retval true  ���_�A�g���r���[�g��ݒ肵��
		* @retval false ���_�A�g���r���[�g�̃p�����[�^���Ȃ�����
		*/
		bool SetVertexAttribute(
			GLuint index, const char* key,
			const json& attributes, const json& accessors,
			const json& bufferViews, const BinaryList& binaryList)
		{
			glEnableVertexAttribArray(index); // ���_�A�g���r���[�g��L����
			const auto attribute = attributes.find(key);
			if (attribute != attributes.end()) {
				// �A�N�Z�T�ƃo�b�t�@�r���[���擾
				const json& accessor = accessors[attribute->get<int>()];
				const int bufferViewId = accessor["bufferView"].get<int>();
				const json& bufferView = bufferViews[bufferViewId];

				// ���_�A�g���r���[�g�̃p�����[�^���擾
				const GLint componentCount = GetComponentCount(accessor);
				const GLenum componentType = accessor["componentType"].get<int>();
				const GLboolean normalized = accessor.value("normalized", false);
				const GLsizei byteStride = GetByteStride(accessor, bufferView);
				const GLsizeiptr offset = GetBinaryDataOffset(accessor, bufferView, binaryList);

				// VAO�ɒ��_�A�g���r���[�g��ݒ�
				glVertexAttribPointer(index, componentCount,
					componentType, normalized, byteStride, reinterpret_cast<void*>(offset));
				return true;
			}
			return false;
		}

		/**
		* VAO�Ƀf�t�H���g�̒��_�A�g���r���[�g��ݒ肷��
		*
		* @param index  �ݒ��̒��_�A�g���r���[�g�ԍ�
		* @param size   �f�[�^�̗v�f��
		* @param offset �f�[�^�̈ʒu
		* @param vbo    ���_�f�[�^��ێ�����VBO
		*/
		void SetDefaultAttribute(GLuint index, GLint size, GLuint offset, GLuint vbo)
		{
			glVertexAttribFormat(index, size, GL_FLOAT, GL_FALSE, offset);
			glVertexAttribBinding(index, index);
			glBindVertexBuffer(index, vbo, 0, 0);
		}

		/**
		* �e�N�X�`����ǂݍ���
		*
		* @param objectName  �e�N�X�`�����̃L�[������
		* @param gltf        glTF�t�@�C����JSON�I�u�W�F�N�g
		* @param parent      �e�N�X�`����������JSON�I�u�W�F�N�g
		* @param foldername  glTF�t�@�C��������t�H���_��
		* @param texDefault  �e�N�X�`�����쐬�ł��Ȃ��ꍇ�ɕԂ��e�N�X�`��
		*
		* @return �ǂݍ��񂾃e�N�X�`���A�܂���texDefault
		*/
		TexturePtr LoadTexture(const char* objectName, const json& gltf, const json& parent,
			const std::string& foldername, const TexturePtr& texDefault)
		{
			// �e�N�X�`�������擾
			const auto textureInfo = parent.find(objectName);
			if (textureInfo == parent.end()) {
				return texDefault;
			}

			// �e�N�X�`���ԍ����擾
			const auto textures = gltf.find("textures");
			const int textureNo = textureInfo->at("index").get<int>();
			if (textures == gltf.end() || textureNo >= textures->size()) {
				return texDefault;
			}

			// �C���[�W�\�[�X�ԍ����擾
			const json& texture = textures->at(textureNo);
			const auto source = texture.find("source");
			if (source == texture.end()) {
				return texDefault;
			}

			// �C���[�W�ԍ����擾
			const auto images = gltf.find("images");
			const int imageNo = source->get<int>();
			if (images == gltf.end() || imageNo >= images->size()) {
				return texDefault;
			}

			// �t�@�C�������擾
			const json& image = images->at(imageNo);
			const auto imageUri = image.find("uri");
			if (imageUri == image.end()) {
				return texDefault;
			}

			// �t�H���_����ǉ����A�g���q��tga�ɕύX
			std::filesystem::path uri = imageUri->get<std::string>();
			std::filesystem::path filename = foldername;
			filename /= uri.parent_path();
			filename /= uri.stem();
			filename += ".tga";

			// �e�N�X�`����ǂݍ���
			return Texture::Create(filename.string(), GL_LINEAR, Texture::Usage::forGltf);
		}

	} // unnamed namespace

	/**
	* glTF���b�V����`�悷��
	*
	* @param mesh      �`�悷�郁�b�V��
	* @param materials �`��Ɏg���}�e���A���z��
	* @param program   �`��v���O����
	*/
	void Draw(const GltfMesh& mesh,
		const std::vector<GltfMaterialPtr>& materials, Shader& shader)
	{
		// ���ׂẴv���~�e�B�u��`��
		for (const auto& prim : mesh.primitives) {
			// �}�e���A����ݒ�
			if (prim.materialNo >= 0 && prim.materialNo < materials.size()) {
				const GltfMaterial& material = *materials[prim.materialNo];

				// �X�y�L�����p�����[�^���v�Z
				const float a = 1.0f - material.roughness;
				const float specularPower = (a * a) * 999 + 1;
				const float normalizeFactor = (specularPower + 8) / (8 * 3.14159263f);
				glProgramUniform4fv(shader, 102, 1, & material.baseColor.x);
				glProgramUniform3f(shader, 103, specularPower, normalizeFactor, 0);

				if (material.texDiffuse) {
					glUniform1i(glGetUniformLocation(shader, "texture_diffuse1"), 0);
					glBindTexture(GL_TEXTURE_2D, *material.texDiffuse);
				}
				if (material.texNormal)
				{
					glUniform1i(glGetUniformLocation(shader, "texture_normal1"), 1);
					glBindTexture(GL_TEXTURE_2D, *material.texNormal);
				}
				if (material.texSpecular)
				{
					glUniform1i(glGetUniformLocation(shader, "texture_speculer1"), 2);
					glBindTexture(GL_TEXTURE_2D, *material.texSpecular);
				}
				if (!material.texNormal && !material.texDiffuse && !material.texSpecular) {
					glBindTextures(0, 1, nullptr);
				}
			}

			glBindVertexArray(*prim.vao);
			glDrawElements(prim.mode, prim.count, prim.type, prim.indices);
		}

		// OpenGL�R���e�L�X�g�ւ̊��蓖�Ă�����
		glBindVertexArray(0);
		glBindTextures(0, 2, nullptr);
	}

	/**
	* �R���X�g���N�^
	*
	* @param bufferCapacity �t�@�C���i�[�p�o�b�t�@�̍ő�o�C�g��
	*/
	GltfFileBuffer::GltfFileBuffer(size_t bufferCapacity)
	{
		// GPU���������m�ۂ��A�������ݐ�p�Ƃ��ă}�b�v����
		buffer = BufferObject::Create(bufferCapacity, nullptr,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		pBuffer = static_cast<uint8_t*>(glMapNamedBuffer(*buffer, GL_WRITE_ONLY));

		// �o�b�t�@�̐擪�Ƀ_�~�[�f�[�^��ݒ�
		const DefaultVertexData defaultData;
		memcpy(pBuffer, &defaultData, sizeof(defaultData));
		curBufferSize = sizeof(defaultData);

		// �f�t�H���g�}�e���A�����쐬
		defaultMaterial = std::make_shared<GltfMaterial>();
		defaultMaterial->name = "defaultMaterial";
		//defaultMaterial->texDiffuse = Texture::Create("defaultTexture", 4, 4);
		static const uint32_t img[4 * 4] = { // �e�N�X�`���f�[�^
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		};
		glTextureSubImage2D(*defaultMaterial->texDiffuse, 0, 0, 0,
			4, 4, GL_BGRA, GL_UNSIGNED_BYTE, img);
	}

	/**
	* glTF�t�@�C������GltfFile�I�u�W�F�N�g���쐬����
	*
	* @param filename glTF�t�@�C����
	*
	* @retval nullptr�ȊO  filename����쐬�����t�@�C���I�u�W�F�N�g
	* @retval nullptr     �ǂݍ��ݎ��s
	*/
	GltfFilePtr GltfFileBuffer::Load(const char* filename)
	{
		// �ȑO�ɓǂݍ��񂾃t�@�C���Ȃ�A�쐬�ς݂̃t�@�C����Ԃ�
		auto itr = files.find(filename);
		if (itr != files.end()) {
			return itr->second;
		}

		// glTF�t�@�C����ǂݍ���
		std::vector<char> buf = ReadFile(filename);
		if (buf.empty()) {
			return nullptr;
		}
		buf.push_back('\0'); // �e�L�X�g�I�[��ǉ�

		// �t�H���_�������o��
		const std::string foldername =
			std::filesystem::path(filename).parent_path().string() + '/';

		// JSON�����
		auto p = Parse(buf.data(), foldername.c_str());
		if (!p) {
			LOG_ERROR("'%s'�̓ǂݍ��݂Ɏ��s���܂���", filename);
			return p;
		}

		// �쐬�����t�@�C����A�z�z��ɒǉ�
		p->name = filename;
		files.emplace(filename, p);

		// �ǂݍ��񂾃t�@�C�����ƃ��b�V�������f�o�b�O���Ƃ��ďo��
		LOG("%s��ǂݍ��݂܂���", filename);
		for (size_t i = 0; i < p->meshes.size(); ++i) {
			LOG(R"(  meshes[%d]="%s")", i, p->meshes[i].name.c_str());

		}
		return p;
	}

	/**
	* JSON�e�L�X�g����GltfFile�I�u�W�F�N�g���쐬����
	*
	* @param text       glTF��JSON�e�L�X�g
	* @param foldername ���\�[�X�ǂݍ��ݗp�̃t�H���_
	*
	* @retval nullptr�ȊO  filename����쐬�����t�@�C���I�u�W�F�N�g
	* @retval nullptr     �ǂݍ��ݎ��s
	*/
	GltfFilePtr GltfFileBuffer::Parse(const char* text, const char* foldername)
	{
		// JSON���
		json gltf = json::parse(text, nullptr, false);
		if (gltf.is_discarded()) {
			LOG_ERROR("JSON�̉�͂Ɏ��s���܂���");
			return nullptr;
		}

		// �o�C�i���t�@�C����ǂݍ���
		const GLsizeiptr prevBufferSize = curBufferSize;
		const json& buffers = gltf["buffers"];
		BinaryList binaryList(buffers.size());
		for (size_t i = 0; i < buffers.size(); ++i) {
			const auto uri = buffers[i].find("uri");
			if (uri == buffers[i].end()) {
				continue;
			}

			// �t�@�C����ǂݍ���
			const std::string binPath = foldername + uri->get<std::string>();
			binaryList[i].bin = ReadFile(binPath.c_str());
			if (binaryList[i].bin.empty()) {
				curBufferSize = prevBufferSize; // �ǂݍ��񂾃f�[�^���Ȃ��������Ƃɂ���
				return nullptr; // �o�C�i���t�@�C���̓ǂݍ��݂Ɏ��s
			}

			// �o�C�i���f�[�^��GPU�������ɃR�s�[
			memcpy(pBuffer + curBufferSize, binaryList[i].bin.data(), binaryList[i].bin.size());

			// �I�t�Z�b�g���X�V
			binaryList[i].offset = curBufferSize;
			curBufferSize += binaryList[i].bin.size();
		}

		// �t�@�C���I�u�W�F�N�g���쐬
		GltfFilePtr file = std::make_shared<GltfFile>();

		// ���[�g�m�[�h�̎p���s����擾
		const auto nodes = gltf.find("nodes");
		if (nodes != gltf.end()) {
			const json& rootNode = nodes->at(0);
			const auto matrix = rootNode.find("matrix");
			if (matrix != rootNode.end()) {
				if (matrix->size() >= 16) {
					for (int i = 0; i < 16; ++i) {
						file->matRoot[i / 4][i % 4] = matrix->at(i).get<float>();
					}

				}
			} // if matrix			
		} // if nodes

		// ���b�V�����쐬
		const json& accessors = gltf["accessors"];
		const json& bufferViews = gltf["bufferViews"];
		const json& meshes = gltf["meshes"];
		file->meshes.reserve(meshes.size());
		for (const json& jsonMesh : meshes) {
			// ���b�V�������擾
			GltfMesh mesh;
			mesh.name = jsonMesh.value("name", "<default>");

			// �v���~�e�B�u���쐬
			const json& primitives = jsonMesh["primitives"];
			mesh.primitives.reserve(primitives.size());
			for (const json& jsonPrim : primitives) {
				// VAO���쐬
				GltfPrimitive prim;
				prim.vao = std::make_shared<VertexArray>();

				// VAO��OpenGL�R���e�L�X�g�Ɋ��蓖�Ă�
				glBindVertexArray(*prim.vao);

				// VBO��IBO���AOpenGL�R���e�L�X�g��VAO�̗����Ɋ��蓖�Ă�
				glBindBuffer(GL_ARRAY_BUFFER, *buffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer);

				// �v���~�e�B�u�̃p�����[�^���擾
				{
					// �v���~�e�B�u�̎��
					prim.mode = jsonPrim.value("mode", GL_TRIANGLES);

					// �C���f�b�N�X���ƌ^
					const int accessorId = jsonPrim["indices"].get<int>();
					const json& accessor = accessors[accessorId];
					prim.count = accessor["count"].get<int>();
					prim.type = accessor["componentType"].get<int>();

					// �C���f�b�N�X�f�[�^�̊J�n�ʒu
					const int bufferViewId = accessor["bufferView"].get<int>();
					const json& bufferView = bufferViews[bufferViewId];

					prim.indices = reinterpret_cast<const GLvoid*>(
						GetBinaryDataOffset(accessor, bufferView, binaryList));
				}

				// ���_�A�g���r���[�g(���_���W)���擾
				const json& attributes = jsonPrim["attributes"];
				const bool hasPosition = SetVertexAttribute(static_cast<GLuint>(AttribIndex::position), "POSITION",
					attributes, accessors, bufferViews, binaryList);
				if (!hasPosition) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::position),
						3, offsetof(DefaultVertexData, position), *buffer);
				}

				// ���_�A�g���r���[�g(�e�N�X�`�����W)���擾
				const bool hasTexcoord0 = SetVertexAttribute(static_cast<GLuint>(AttribIndex::texcoord0), "TEXCOORD_0",
					attributes, accessors, bufferViews, binaryList);
				if (!hasTexcoord0) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::texcoord0),
						2, offsetof(DefaultVertexData, texcoord0), *buffer);
				}

				// ���_�A�g���r���[�g(�@��)���擾
				const bool hasNormal = SetVertexAttribute(static_cast<GLuint>(AttribIndex::normal), "NORMAL",
					attributes, accessors, bufferViews, binaryList);
				if (!hasNormal) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::normal),
						3, offsetof(DefaultVertexData, normal), *buffer);
				}

				// ���_�A�g���r���[�g(�^���W�F���g)���擾
				const bool hasTangent = SetVertexAttribute(static_cast<GLuint>(AttribIndex::tangent), "TANGENT",
					attributes, accessors, bufferViews, binaryList);
				if (!hasTangent) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::tangent),
						4, offsetof(DefaultVertexData, tangent), *buffer);
				}

				// �v���~�e�B�u���g�p����}�e���A���ԍ����擾
				prim.materialNo = jsonPrim.value("material", 0);

				// VAO, VBO, IBO��OpenGL�R���e�L�X�g�ւ̊��蓖�Ă�����
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// �쐬�����v���~�e�B�u��z��ɒǉ�
				mesh.primitives.push_back(prim);

			}

			// �쐬�������b�V���I�u�W�F�N�g��z��ɒǉ�
			file->meshes.push_back(mesh);
		}



		// �}�e���A�����쐬
		const auto materials = gltf.find("materials");
		if (materials != gltf.end()) {
			file->materials.reserve(materials->size());
			for (const json& material : *materials) {
				GltfMaterialPtr m = std::make_shared<GltfMaterial>();

				// ���O��ݒ�
				m->name = material.value("name", std::string());
				// �J���[��ݒ�
				const auto pbr = material.find("pbrMetallicRoughness");
				if (pbr != material.end()) {

					// �}�e���A���J���[���擾
					m->baseColor = glm::vec4(1);
					const auto baseColorFactor = pbr->find("baseColorFactor");
					if (baseColorFactor != pbr->end()) {
						for (int i = 0; i < baseColorFactor->size(); ++i) {
							m->baseColor[i] = baseColorFactor->at(i).get<float>();
						}
					}

					// �J���[�e�N�X�`����ǂݍ���
					m->texDiffuse = LoadTexture("baseColorTexture", gltf, *pbr,
						foldername, defaultMaterial->texDiffuse);
				} // if pbr

				// �@���e�N�X�`����ǂݍ���
				m->texNormal = LoadTexture("normalTexture", gltf, material,
					foldername, nullptr);

				// ���˃e�N�X�`����ǂݍ���
				m->texSpecular = LoadTexture("specularTexture", gltf, material,
					foldername, nullptr);

				// �쐬�����}�e���A����ǉ�
				file->materials.push_back(m);
			}

		} // if materials

		return file; // �쐬�����t�@�C���I�u�W�F�N�g��Ԃ�
	}

} // namespace Mesh