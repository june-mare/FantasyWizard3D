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
	* glTFのマテリアル
	*/
	struct GltfMaterial
	{
		std::string name = "";				// マテリアル名
		glm::vec4 baseColor = glm::vec4(1); // 基本色
		TexturePtr texDiffuse = nullptr;	// 基本色テクスチャ
		TexturePtr texNormal = nullptr;     // 法線テクスチャ
		TexturePtr texSpecular = nullptr;   // 法線テクスチャ
		float roughness = 1;				// 表面の粗さ。粗い=1 なめらか=0
	};
	using GltfMaterialPtr = std::shared_ptr<GltfMaterial>;

	/**
	* プリミティブデータ
	*/
	struct GltfPrimitive
	{
		GLenum mode = GL_TRIANGLES;		// プリミティブの種類
		GLsizei count = 0;				// 描画するインデックス数
		GLenum type = GL_UNSIGNED_SHORT;// インデックスデータ型
		const GLvoid* indices = 0;		// 描画開始インデックスのバイトオフセット
		GLint baseVertex = 0;			// インデックス0番とみなされる頂点配列内の位置

		VertexArrayPtr vao = nullptr;	// プリミティブ用VAO
		size_t materialNo = 0;			// マテリアル番号
	};

	/**
	* メッシュデータ
	*/
	struct GltfMesh
	{
		std::string name = "";						// メッシュ名
		std::vector<GltfPrimitive> primitives = {}; // プリミティブ配列
	};

	/**
	* ファイル
	*/
	struct GltfFile
	{
		std::string name = "";							// ファイル名
		std::vector<GltfMesh> meshes = {};				// メッシュ配列
		std::vector<GltfMaterialPtr> materials = {};	// マテリアル配列
		glm::mat4 matRoot = glm::mat4(1);				// 基本姿勢行列
	};
	using GltfFilePtr = std::shared_ptr<GltfFile>;

	void Draw(const GltfMesh& mesh,
		const std::vector<GltfMaterialPtr>&materials, Shader& shader);

	/**
	* glTFファイルから読み込んだメッシュを管理するクラス
	*/
	class GltfFileBuffer
	{
	public:
		// glTFファイルバッファを作成する
		static std::shared_ptr<GltfFileBuffer> Create(size_t bufferCapacity) {
			return std::make_shared<GltfFileBuffer>(bufferCapacity);

		}

		// コンストラクタ・デストラクタ
		explicit GltfFileBuffer(size_t bufferCapacity);
		~GltfFileBuffer() = default;

		// ファイルの読み込み
		GltfFilePtr Load(const char* filename);

	private:
		GltfFilePtr Parse(const char* text, const char* foldername);

		BufferObjectPtr buffer = nullptr;          // binファイルを読み込むバッファ
		uint8_t* pBuffer = nullptr;				   // マップされたアドレス
		GLsizeiptr curBufferSize = 0;              // 書き込み済みデータ数
		GltfMaterialPtr defaultMaterial;           // マテリアル未設定時に使用するマテリアル

		// ファイル管理用の連想配列
		std::unordered_map<std::string, GltfFilePtr> files;

	};
	using GltfFileBufferPtr = std::shared_ptr<GltfFileBuffer>;



}//GLTFMesh