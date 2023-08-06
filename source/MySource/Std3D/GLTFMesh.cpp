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
		* バイナリデータ
		*/
		struct BinaryData
		{
			GLsizeiptr offset;     // GPUメモリ上のデータ開始オフセット
			std::vector<char> bin; // CPUメモリに読み込んだデータ
		};
		using BinaryList = std::vector<BinaryData>;

		/**
		* ファイルを読み込む
		*
		* @param filename 読み込むファイル名
		*
		* @return 読み込んだデータ配列
		*/
		std::vector<char> ReadFile(const char* filename)
		{
			std::ifstream file(filename, std::ios::binary);
			if (!file) {
				LOG_WARNING("%sを開けません", filename);
				return {};

			}
			std::vector<char> buf(std::filesystem::file_size(filename));
			file.read(buf.data(), buf.size());
			return buf;
		}

		/**
		* 頂点アトリビュート番号
		*/
		enum class AttribIndex
		{
			position,
			texcoord0,
			normal,
			tangent,
		};

		/**
		* 必要な頂点データ要素がプリミティブに存在しない場合に使うデータ
		*/
		struct DefaultVertexData
		{
			glm::vec3 position = glm::vec3(0);
			glm::vec2 texcoord0 = glm::vec2(0);
			glm::vec3 normal = glm::vec3(0, 0, -1);
			glm::vec4 tangent = glm::vec4(1, 0, 0, 1);
		};


		/**
		* 成分の数を取得する
		*
		* @param accessor パラメータを持つアクセサ
		*
		* @return 成分の数
		*/
		int GetComponentCount(const json& accessor)
		{
			// 型名と成分数の対応表
			static const struct {
				const char* type;   // 型の名前
				int componentCount; // 成分数

			} componentCountList[] = {
				{ "SCALAR", 1 },
				{ "VEC2", 2 }, { "VEC3", 3 }, { "VEC4", 4 },
				{ "MAT2", 4 }, { "MAT3", 9 }, { "MAT4", 16 },
			};

			// 対応表から成分数を取得
			const std::string& type = accessor["type"].get<std::string>();
			for (const auto& e : componentCountList) {
				if (type == e.type) {
					return e.componentCount;

				}

			}
			LOG_WARNING("不明な型`%s`が指定されています", type.c_str());
			return 1; // glTFの仕様どおりなら、ここに来ることはないはず
		}

		/**
		* ストライド(次の要素までの距離)を取得する
		*
		* @param accessor   アクセサ
		* @param bufferView バッファビュー
		*
		* @return ストライド
		*/
		GLsizei GetByteStride(const json& accessor, const json& bufferView)
		{
			// byteStrideが定義されていたら、その値を返す
			const auto byteStride = bufferView.find("byteStride");
			if (byteStride != bufferView.end()) {
				return byteStride->get<int>();
			}

			// byteStrideが未定義の場合、要素1個分のサイズをストライドとする
			// 参考: glTF-2.0仕様 3.6.2.4. Data Alignment

			int componentSize = 1; // 成分型のサイズ
			const int componentType = accessor["componentType"].get<int>();
			switch (componentType) {
			case GL_BYTE:           componentSize = 1; break;
			case GL_UNSIGNED_BYTE:  componentSize = 1; break;
			case GL_SHORT:          componentSize = 2; break;
			case GL_UNSIGNED_SHORT: componentSize = 2; break;
			case GL_UNSIGNED_INT:   componentSize = 4; break;
			case GL_FLOAT:          componentSize = 4; break;
			default:
				LOG_WARNING("glTFの仕様にない型%dが使われています", componentType);
				break;

			}

			// ストライド = 成分型のサイズ * 成分数
			const int componentCount = GetComponentCount(accessor);
			return componentSize * componentCount;
		}

		/**
		* データの開始位置を取得する
		*
		* @param accessor   アクセサ
		* @param bufferView バッファビュー
		* @param binaryList バイナリデータ配列
		*
		* @return データの開始位置
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
		* 頂点アトリビュートを設定する
		*
		* @retval true  頂点アトリビュートを設定した
		* @retval false 頂点アトリビュートのパラメータがなかった
		*/
		bool SetVertexAttribute(
			GLuint index, const char* key,
			const json& attributes, const json& accessors,
			const json& bufferViews, const BinaryList& binaryList)
		{
			glEnableVertexAttribArray(index); // 頂点アトリビュートを有効化
			const auto attribute = attributes.find(key);
			if (attribute != attributes.end()) {
				// アクセサとバッファビューを取得
				const json& accessor = accessors[attribute->get<int>()];
				const int bufferViewId = accessor["bufferView"].get<int>();
				const json& bufferView = bufferViews[bufferViewId];

				// 頂点アトリビュートのパラメータを取得
				const GLint componentCount = GetComponentCount(accessor);
				const GLenum componentType = accessor["componentType"].get<int>();
				const GLboolean normalized = accessor.value("normalized", false);
				const GLsizei byteStride = GetByteStride(accessor, bufferView);
				const GLsizeiptr offset = GetBinaryDataOffset(accessor, bufferView, binaryList);

				// VAOに頂点アトリビュートを設定
				glVertexAttribPointer(index, componentCount,
					componentType, normalized, byteStride, reinterpret_cast<void*>(offset));
				return true;
			}
			return false;
		}

		/**
		* VAOにデフォルトの頂点アトリビュートを設定する
		*
		* @param index  設定先の頂点アトリビュート番号
		* @param size   データの要素数
		* @param offset データの位置
		* @param vbo    頂点データを保持するVBO
		*/
		void SetDefaultAttribute(GLuint index, GLint size, GLuint offset, GLuint vbo)
		{
			glVertexAttribFormat(index, size, GL_FLOAT, GL_FALSE, offset);
			glVertexAttribBinding(index, index);
			glBindVertexBuffer(index, vbo, 0, 0);
		}

		/**
		* テクスチャを読み込む
		*
		* @param objectName  テクスチャ情報のキー文字列
		* @param gltf        glTFファイルのJSONオブジェクト
		* @param parent      テクスチャ情報を持つJSONオブジェクト
		* @param foldername  glTFファイルがあるフォルダ名
		* @param texDefault  テクスチャが作成できない場合に返すテクスチャ
		*
		* @return 読み込んだテクスチャ、またはtexDefault
		*/
		TexturePtr LoadTexture(const char* objectName, const json& gltf, const json& parent,
			const std::string& foldername, const TexturePtr& texDefault)
		{
			// テクスチャ情報を取得
			const auto textureInfo = parent.find(objectName);
			if (textureInfo == parent.end()) {
				return texDefault;
			}

			// テクスチャ番号を取得
			const auto textures = gltf.find("textures");
			const int textureNo = textureInfo->at("index").get<int>();
			if (textures == gltf.end() || textureNo >= textures->size()) {
				return texDefault;
			}

			// イメージソース番号を取得
			const json& texture = textures->at(textureNo);
			const auto source = texture.find("source");
			if (source == texture.end()) {
				return texDefault;
			}

			// イメージ番号を取得
			const auto images = gltf.find("images");
			const int imageNo = source->get<int>();
			if (images == gltf.end() || imageNo >= images->size()) {
				return texDefault;
			}

			// ファイル名を取得
			const json& image = images->at(imageNo);
			const auto imageUri = image.find("uri");
			if (imageUri == image.end()) {
				return texDefault;
			}

			// フォルダ名を追加し、拡張子をtgaに変更
			std::filesystem::path uri = imageUri->get<std::string>();
			std::filesystem::path filename = foldername;
			filename /= uri.parent_path();
			filename /= uri.stem();
			filename += ".tga";

			// テクスチャを読み込む
			return Texture::Create(filename.string(), GL_LINEAR, Texture::Usage::forGltf);
		}

	} // unnamed namespace

	/**
	* glTFメッシュを描画する
	*
	* @param mesh      描画するメッシュ
	* @param materials 描画に使うマテリアル配列
	* @param program   描画プログラム
	*/
	void Draw(const GltfMesh& mesh,
		const std::vector<GltfMaterialPtr>& materials, Shader& shader)
	{
		// すべてのプリミティブを描画
		for (const auto& prim : mesh.primitives) {
			// マテリアルを設定
			if (prim.materialNo >= 0 && prim.materialNo < materials.size()) {
				const GltfMaterial& material = *materials[prim.materialNo];

				// スペキュラパラメータを計算
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

		// OpenGLコンテキストへの割り当てを解除
		glBindVertexArray(0);
		glBindTextures(0, 2, nullptr);
	}

	/**
	* コンストラクタ
	*
	* @param bufferCapacity ファイル格納用バッファの最大バイト数
	*/
	GltfFileBuffer::GltfFileBuffer(size_t bufferCapacity)
	{
		// GPUメモリを確保し、書き込み専用としてマップする
		buffer = BufferObject::Create(bufferCapacity, nullptr,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		pBuffer = static_cast<uint8_t*>(glMapNamedBuffer(*buffer, GL_WRITE_ONLY));

		// バッファの先頭にダミーデータを設定
		const DefaultVertexData defaultData;
		memcpy(pBuffer, &defaultData, sizeof(defaultData));
		curBufferSize = sizeof(defaultData);

		// デフォルトマテリアルを作成
		defaultMaterial = std::make_shared<GltfMaterial>();
		defaultMaterial->name = "defaultMaterial";
		//defaultMaterial->texDiffuse = Texture::Create("defaultTexture", 4, 4);
		static const uint32_t img[4 * 4] = { // テクスチャデータ
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff,
		};
		glTextureSubImage2D(*defaultMaterial->texDiffuse, 0, 0, 0,
			4, 4, GL_BGRA, GL_UNSIGNED_BYTE, img);
	}

	/**
	* glTFファイルからGltfFileオブジェクトを作成する
	*
	* @param filename glTFファイル名
	*
	* @retval nullptr以外  filenameから作成したファイルオブジェクト
	* @retval nullptr     読み込み失敗
	*/
	GltfFilePtr GltfFileBuffer::Load(const char* filename)
	{
		// 以前に読み込んだファイルなら、作成済みのファイルを返す
		auto itr = files.find(filename);
		if (itr != files.end()) {
			return itr->second;
		}

		// glTFファイルを読み込む
		std::vector<char> buf = ReadFile(filename);
		if (buf.empty()) {
			return nullptr;
		}
		buf.push_back('\0'); // テキスト終端を追加

		// フォルダ名を取り出す
		const std::string foldername =
			std::filesystem::path(filename).parent_path().string() + '/';

		// JSONを解析
		auto p = Parse(buf.data(), foldername.c_str());
		if (!p) {
			LOG_ERROR("'%s'の読み込みに失敗しました", filename);
			return p;
		}

		// 作成したファイルを連想配列に追加
		p->name = filename;
		files.emplace(filename, p);

		// 読み込んだファイル名とメッシュ名をデバッグ情報として出力
		LOG("%sを読み込みました", filename);
		for (size_t i = 0; i < p->meshes.size(); ++i) {
			LOG(R"(  meshes[%d]="%s")", i, p->meshes[i].name.c_str());

		}
		return p;
	}

	/**
	* JSONテキストからGltfFileオブジェクトを作成する
	*
	* @param text       glTFのJSONテキスト
	* @param foldername リソース読み込み用のフォルダ
	*
	* @retval nullptr以外  filenameから作成したファイルオブジェクト
	* @retval nullptr     読み込み失敗
	*/
	GltfFilePtr GltfFileBuffer::Parse(const char* text, const char* foldername)
	{
		// JSON解析
		json gltf = json::parse(text, nullptr, false);
		if (gltf.is_discarded()) {
			LOG_ERROR("JSONの解析に失敗しました");
			return nullptr;
		}

		// バイナリファイルを読み込む
		const GLsizeiptr prevBufferSize = curBufferSize;
		const json& buffers = gltf["buffers"];
		BinaryList binaryList(buffers.size());
		for (size_t i = 0; i < buffers.size(); ++i) {
			const auto uri = buffers[i].find("uri");
			if (uri == buffers[i].end()) {
				continue;
			}

			// ファイルを読み込む
			const std::string binPath = foldername + uri->get<std::string>();
			binaryList[i].bin = ReadFile(binPath.c_str());
			if (binaryList[i].bin.empty()) {
				curBufferSize = prevBufferSize; // 読み込んだデータをなかったことにする
				return nullptr; // バイナリファイルの読み込みに失敗
			}

			// バイナリデータをGPUメモリにコピー
			memcpy(pBuffer + curBufferSize, binaryList[i].bin.data(), binaryList[i].bin.size());

			// オフセットを更新
			binaryList[i].offset = curBufferSize;
			curBufferSize += binaryList[i].bin.size();
		}

		// ファイルオブジェクトを作成
		GltfFilePtr file = std::make_shared<GltfFile>();

		// ルートノードの姿勢行列を取得
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

		// メッシュを作成
		const json& accessors = gltf["accessors"];
		const json& bufferViews = gltf["bufferViews"];
		const json& meshes = gltf["meshes"];
		file->meshes.reserve(meshes.size());
		for (const json& jsonMesh : meshes) {
			// メッシュ名を取得
			GltfMesh mesh;
			mesh.name = jsonMesh.value("name", "<default>");

			// プリミティブを作成
			const json& primitives = jsonMesh["primitives"];
			mesh.primitives.reserve(primitives.size());
			for (const json& jsonPrim : primitives) {
				// VAOを作成
				GltfPrimitive prim;
				prim.vao = std::make_shared<VertexArray>();

				// VAOをOpenGLコンテキストに割り当てる
				glBindVertexArray(*prim.vao);

				// VBOとIBOを、OpenGLコンテキストとVAOの両方に割り当てる
				glBindBuffer(GL_ARRAY_BUFFER, *buffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer);

				// プリミティブのパラメータを取得
				{
					// プリミティブの種類
					prim.mode = jsonPrim.value("mode", GL_TRIANGLES);

					// インデックス数と型
					const int accessorId = jsonPrim["indices"].get<int>();
					const json& accessor = accessors[accessorId];
					prim.count = accessor["count"].get<int>();
					prim.type = accessor["componentType"].get<int>();

					// インデックスデータの開始位置
					const int bufferViewId = accessor["bufferView"].get<int>();
					const json& bufferView = bufferViews[bufferViewId];

					prim.indices = reinterpret_cast<const GLvoid*>(
						GetBinaryDataOffset(accessor, bufferView, binaryList));
				}

				// 頂点アトリビュート(頂点座標)を取得
				const json& attributes = jsonPrim["attributes"];
				const bool hasPosition = SetVertexAttribute(static_cast<GLuint>(AttribIndex::position), "POSITION",
					attributes, accessors, bufferViews, binaryList);
				if (!hasPosition) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::position),
						3, offsetof(DefaultVertexData, position), *buffer);
				}

				// 頂点アトリビュート(テクスチャ座標)を取得
				const bool hasTexcoord0 = SetVertexAttribute(static_cast<GLuint>(AttribIndex::texcoord0), "TEXCOORD_0",
					attributes, accessors, bufferViews, binaryList);
				if (!hasTexcoord0) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::texcoord0),
						2, offsetof(DefaultVertexData, texcoord0), *buffer);
				}

				// 頂点アトリビュート(法線)を取得
				const bool hasNormal = SetVertexAttribute(static_cast<GLuint>(AttribIndex::normal), "NORMAL",
					attributes, accessors, bufferViews, binaryList);
				if (!hasNormal) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::normal),
						3, offsetof(DefaultVertexData, normal), *buffer);
				}

				// 頂点アトリビュート(タンジェント)を取得
				const bool hasTangent = SetVertexAttribute(static_cast<GLuint>(AttribIndex::tangent), "TANGENT",
					attributes, accessors, bufferViews, binaryList);
				if (!hasTangent) {
					SetDefaultAttribute(static_cast<GLuint>(AttribIndex::tangent),
						4, offsetof(DefaultVertexData, tangent), *buffer);
				}

				// プリミティブが使用するマテリアル番号を取得
				prim.materialNo = jsonPrim.value("material", 0);

				// VAO, VBO, IBOのOpenGLコンテキストへの割り当てを解除
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// 作成したプリミティブを配列に追加
				mesh.primitives.push_back(prim);

			}

			// 作成したメッシュオブジェクトを配列に追加
			file->meshes.push_back(mesh);
		}



		// マテリアルを作成
		const auto materials = gltf.find("materials");
		if (materials != gltf.end()) {
			file->materials.reserve(materials->size());
			for (const json& material : *materials) {
				GltfMaterialPtr m = std::make_shared<GltfMaterial>();

				// 名前を設定
				m->name = material.value("name", std::string());
				// カラーを設定
				const auto pbr = material.find("pbrMetallicRoughness");
				if (pbr != material.end()) {

					// マテリアルカラーを取得
					m->baseColor = glm::vec4(1);
					const auto baseColorFactor = pbr->find("baseColorFactor");
					if (baseColorFactor != pbr->end()) {
						for (int i = 0; i < baseColorFactor->size(); ++i) {
							m->baseColor[i] = baseColorFactor->at(i).get<float>();
						}
					}

					// カラーテクスチャを読み込む
					m->texDiffuse = LoadTexture("baseColorTexture", gltf, *pbr,
						foldername, defaultMaterial->texDiffuse);
				} // if pbr

				// 法線テクスチャを読み込む
				m->texNormal = LoadTexture("normalTexture", gltf, material,
					foldername, nullptr);

				// 反射テクスチャを読み込む
				m->texSpecular = LoadTexture("specularTexture", gltf, material,
					foldername, nullptr);

				// 作成したマテリアルを追加
				file->materials.push_back(m);
			}

		} // if materials

		return file; // 作成したファイルオブジェクトを返す
	}

} // namespace Mesh