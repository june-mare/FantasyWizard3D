#include "Texture.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include "../Debug/Debug.h"
#include "../../stbi/stb_image.h"

TexturePtr Texture::Create(const std::string& filename, GLenum filterMode, Usage usage)
{
	return std::make_shared<Texture>(filename, filterMode, usage);
}

TexturePtr Texture::Create(const char* name, GLint width, GLint height, GLenum internalFormat, GLenum filterMode, Type type)
{
	return std::make_shared<Texture>(
		name, width, height, internalFormat, filterMode,type);
}

Texture::Texture(const std::string& filename, GLenum filterMode, Usage usage)
	:filename(filename)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (usage == Usage::for2D ||
		usage == Usage::forGltf ||
		usage == Usage::forGltfLinear) {

	}
	int nrComponents;

	auto data = stbi_load(filename.c_str(), &width
		, &height, &nrComponents, 0);

	if (data) {
		GLenum format = {};
		/*if (usage == Usage::for3DLinear || usage == Usage::forGltfLinear) {
			format = GL_RGBA8;
		}*/
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTextureSubImage2D(textureID, 0, 0, 0, width, height,
			GL_BGRA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		//LOG("テキスチャを読み込みました %s", filename.c_str());
		stbi_image_free(data);
	}
	else {
		LOG_ERROR("Texture failed to load at path; %s", filename);
		stbi_image_free(data);
	}

	tex = textureID;
}

Texture::~Texture()
{
	glDeleteTextures(1, &tex);
}

Texture::Texture(const char* name, GLint width, GLint height, GLenum internalFormat, GLenum filterMode,Type type)
	:width(width), height(height)
{
	// テクスチャを作成
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureStorage2D(tex, 1, internalFormat, width, height);

	// 拡大縮小フィルタを設定
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filterMode);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filterMode);
	if (type == Type::depth){
		glTextureParameteri(tex, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(tex, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
}

void Texture::SetWrapMode(GLenum wrapMode)
{
	glTextureParameteri(tex, GL_TEXTURE_WRAP_S, wrapMode);
	glTextureParameteri(tex, GL_TEXTURE_WRAP_T, wrapMode);
}
