#pragma once
#include <glad/glad.h>
#include <memory>

// 先行宣言
class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

/**
* VAOを管理するクラス
*/
class VertexArray
{
public:
	// VAOを作成
	static VertexArrayPtr Create() { return std::make_shared<VertexArray>(); }

	// コンストラクタ、デストラクタ
	VertexArray() { glCreateVertexArrays(1, &vao); }
	~VertexArray() { glDeleteVertexArrays(1, &vao); }

	// コピーと代入を禁止
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	// 管理番号を取得
	operator GLuint() const { return vao; }

private:
	GLuint vao = 0;
};