#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>

// 先行宣言
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/**
* テクスチャ
*/
class Texture
{
public:
	enum class Usage
	{
		for2D, // 2D描画用
		for3D, // 3D描画用
		for3DLinear, // 3D描画用(リニア色空間)
		forGltf, // glTF描画用
		forGltfLinear, // glTF描画用(リニア色空間)
	};

	enum class Type {
		color,
		depth,
	};

	// テクスチャを作成
	static TexturePtr Create(
		const std::string& filename, GLenum filterMode, Usage usage);

	static TexturePtr Create(
		const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode,Type type = Type::color);

	// コンストラクタ、デストラクタ
	Texture(const std::string& filename, GLenum filterMode = GL_NEAREST, Usage usage = Usage::for2D);
	~Texture();

	Texture(const char* name, GLint width, GLint height,
		GLenum internalFormat, GLenum filterMode,Type type = Type::color);

	// コピーと代入を禁止
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void SetWrapMode(GLenum wrapMode);
	const GLuint GetTex(){ return tex; }
	// 管理番号を取得
	operator GLuint() const { return tex; }
	//ファイルネームを取得
	inline const std::string GetName()noexcept { return filename; }

public:	
	int		width = 0, height = 0;
private:
	GLuint		tex = 0;
	std::string filename = {};
};
