#pragma once
#include <glad/glad.h>
#include <memory>
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

// ��s�錾
class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

/**
* �v���O�����p�C�v���C�����Ǘ�����N���X
*/
class Shader
{
public:
	// �v���O�����p�C�v���C�����쐬
	static ShaderPtr Create(
		const std::string& filenameVS, const std::string& filenameFS, const char* filenameGS = "");

	// �R���X�g���N�^�A�f�X�g���N�^
	Shader(
		const std::string& filenameVS, const std::string& filenameFS, const char* filenameGS = "");
	~Shader();

	// �R�s�[�Ƒ�����֎~
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	// �Ǘ��ԍ����擾
	operator GLuint() const { return id; }
	GLuint GetVS() const { return vs; }
	GLuint GetFS() const { return fs; }
	GLuint GetGS() const { return gs; }
	void Use()const noexcept;
	void SetBool(const std::string& name, bool value)const;
	void SetInt(const std::string& name, int value)const;
	void SetFloat(const std::string& name, float value)const;
	void SetFloat(const std::string& name, float value1, float value2)const;
	void SetFloat(const std::string& name, glm::vec2& value)const;
	void SetFloat(const std::string& name, float value1, float value2, float value3)const;
	void SetFloat(const std::string& name, glm::vec3& value)const;
	void SetFloat(const std::string& name, float value1, float value2, float value3, float value4)const;
	void SetFloat(const std::string& name, glm::vec4& value)const;
	void SetMat2(const std::string& name, const glm::mat2& mat)const;
	void SetMat3(const std::string& name, const glm::mat3& mat)const;
	void SetMat4(const std::string& name, const glm::mat4& mat)const;
	void UniformBlockBinding(const std::string& name);

	GLuint id = 0; // �v���O�����p�C�v���C��
	std::string name = {};
private:
	void CheckCompileErrors(GLuint shader, std::string type);
	GLuint vs = 0; // ���_�V�F�[�_
	GLuint fs = 0; // �t���O�����g�V�F�[�_
	GLuint gs = 0; // �t���O�����g�V�F�[�_
	std::string filenameVS; // ���_�V�F�[�_�t�@�C����
	std::string filenameFS; // �t���O�����g�V�F�[�_�t�@�C����
	std::string filenameGS; // �W�I���g���|�V�F�[�_�t�@�C����
};