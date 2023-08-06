#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>

// ��s�錾
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/**
* �e�N�X�`��
*/
class Texture
{
public:
	enum class Usage
	{
		for2D, // 2D�`��p
		for3D, // 3D�`��p
		for3DLinear, // 3D�`��p(���j�A�F���)
		forGltf, // glTF�`��p
		forGltfLinear, // glTF�`��p(���j�A�F���)
	};

	enum class Type {
		color,
		depth,
	};

	// �e�N�X�`�����쐬
	static TexturePtr Create(
		const std::string& filename, GLenum filterMode, Usage usage);

	static TexturePtr Create(
		const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode,Type type = Type::color);

	// �R���X�g���N�^�A�f�X�g���N�^
	Texture(const std::string& filename, GLenum filterMode = GL_NEAREST, Usage usage = Usage::for2D);
	~Texture();

	Texture(const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode,Type type = Type::color);

	// �R�s�[�Ƒ�����֎~
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void SetWrapMode(GLenum wrapMode);
	const GLuint GetTex(){ return tex; }
	// �Ǘ��ԍ����擾
	operator GLuint() const { return tex; }
	//�t�@�C���l�[�����擾
	inline const std::string GetName()noexcept { return filename; }

public:	
	int		width = 0, height = 0;
private:
	GLuint		tex = 0;
	std::string filename = {};
};
