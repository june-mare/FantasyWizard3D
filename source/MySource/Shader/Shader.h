#pragma once
#include <glad/glad.h>
#include <memory>
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

// 先行宣言
class Shader;
using ShaderPtr = std::shared_ptr<Shader>;

/**
* プログラムパイプラインを管理するクラス
*/
class Shader
{
public:
	// プログラムパイプラインを作成
	static ShaderPtr Create(
		const std::string& filenameVS, const std::string& filenameFS, const char* filenameGS = "");

	// コンストラクタ、デストラクタ
	Shader(
		const std::string& filenameVS, const std::string& filenameFS, const char* filenameGS = "");
	~Shader();

	// コピーと代入を禁止
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	// 管理番号を取得
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

	GLuint id = 0; // プログラムパイプライン
	std::string name = {};
private:
	void CheckCompileErrors(GLuint shader, std::string type);
	GLuint vs = 0; // 頂点シェーダ
	GLuint fs = 0; // フラグメントシェーダ
	GLuint gs = 0; // フラグメントシェーダ
	std::string filenameVS; // 頂点シェーダファイル名
	std::string filenameFS; // フラグメントシェーダファイル名
	std::string filenameGS; // ジオメトリ－シェーダファイル名
};