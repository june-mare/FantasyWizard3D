#define _CRT_SECURE_NO_WARNINGS
#include "StdMesh.h"
#include "Debug/Debug.h"
#include "BufferObject/VertexArray.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdio.h>


namespace StdMesh
{
	void Draw(const Primitive& prim)
	{
		glDrawElementsInstancedBaseVertex(prim.mode, prim.count,
			GL_UNSIGNED_SHORT, prim.indices, 1, prim.baseVertex);
	}

	void Draw(const StaticMesh& mesh, GLuint program)
	{
		for (const auto& prim : mesh.primitives) {
			// マテリアルを設定
			if (prim.materialNo >= 0 && prim.materialNo < mesh.materials.size()) {
				const Material& material = *mesh.materials[prim.materialNo];

				for (int i = 0; i < material.textures.size();i++) {


				}

				if (material.texDiffuse) {
					glUniform1i(glGetUniformLocation(program, "texture_diffuse1"),0);
					glBindTexture(GL_TEXTURE_2D, *material.texDiffuse);
				}
				if (material.texNormal)
				{
					glUniform1i(glGetUniformLocation(program, "texture_normal1"),1);
					glBindTexture(GL_TEXTURE_2D, *material.texNormal);
				}
				if (material.texSpeculer)
				{
					glUniform1i(glGetUniformLocation(program, "texture_speculer1"), 2);
					glBindTexture(GL_TEXTURE_2D, *material.texSpeculer);
				}
				if(!material.texNormal && !material.texDiffuse && !material.texSpeculer) {
					glBindTextures(0, 1, nullptr);
				}
			}
			Draw(prim);
		}
	}

	void Draw(const StaticMesh& mesh, std::vector<StdMesh::MaterialPtr> materials, GLuint program)
	{
		for (const auto& prim : mesh.primitives) {
			// マテリアルを設定
			if (prim.materialNo >= 0 && prim.materialNo < mesh.materials.size()) {
				const Material& material = *mesh.materials[prim.materialNo];				
				if (material.texDiffuse) {
					glUniform1i(glGetUniformLocation(program, "texture_diffuse1"), 0);
					glBindTexture(GL_TEXTURE_2D, *material.texDiffuse);
				}
				if (material.texNormal)
				{
					glUniform1i(glGetUniformLocation(program, "texture_normal1"), 1);
					glBindTexture(GL_TEXTURE_2D, *material.texNormal);
				}
				if (material.texSpeculer)
				{
					glUniform1i(glGetUniformLocation(program, "texture_speculer1"), 2);
					glBindTexture(GL_TEXTURE_2D, *material.texSpeculer);
				}
				if (!material.texNormal && !material.texDiffuse && !material.texSpeculer) {
					glBindTextures(0, 1, nullptr);
				}
			}
			Draw(prim);
		}
	}

	//void Draw(const StaticMesh& mesh ,std::vector<StdMesh::MaterialPtr> materials , GLuint program)
	//{
	//	for (const auto& prim : mesh.primitives) {
	//		// マテリアルを設定
	//		if (prim.materialNo >= 0 && prim.materialNo < mesh.materials.size()) {
	//			const Material& material = *materials[prim.materialNo];
	//			if (program) {
	//				glProgramUniform4fv(program, 102, 1, &material.baseColor.x);
	//				glProgramUniform2f(program, 103,
	//					material.specularPower, material.normalizeFactor);
	//			}
	//			if (material.texDiffuse) {
	//				glUniform1i(glGetUniformLocation(program, "texture_diffuse1"), 0);
	//				glBindTexture(GL_TEXTURE_2D, *material.texDiffuse);
	//			}
	//			if (material.texNormal)
	//			{
	//				glUniform1i(glGetUniformLocation(program, "texture_normal1"), 1);
	//				glBindTexture(GL_TEXTURE_2D, *material.texNormal);
	//			}
	//			if (material.texSpeculer)
	//			{
	//				glUniform1i(glGetUniformLocation(program, "texture_speculer1"), 2);
	//				glBindTexture(GL_TEXTURE_2D, *material.texSpeculer);
	//			}
	//			if (!material.texNormal && !material.texDiffuse && !material.texSpeculer) {
	//				glBindTextures(0, 1, nullptr);
	//			}
	//		}
	//		Draw(prim);
	//	}
	//}



	/*
	* MTLファイルを読み込む
	*
	* @param foldername OBJファイルのあるフォルダ名
	* @param filename   MTLファイル名
	*
	* @return MTLファイルに含まれるマテリアルの配列
	*/
	std::vector<MaterialPtr> LoadMTL(
		const std::string& foldername, const char* filename)
	{
		// MTLファイルを開く
		const std::string fullpath = foldername + filename;
		std::ifstream file(fullpath);
		if (!file) {
			LOG_ERROR("%sを開けません", fullpath.c_str());
			return {};
		}

		// MTLファイルを解析する
		std::vector<MaterialPtr> materials;
		MaterialPtr pMaterial;
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);

			// マテリアル定義の読み取りを試みる
			char name[1000] = { 0 };
			if (sscanf(line.data(), " newmtl %999s", name) == 1) {
				pMaterial = std::make_shared<Material>();
				pMaterial->name = name;
				materials.push_back(pMaterial);
				continue;
			}

			// マテリアルが定義されていない場合は行を無視する
			if (!pMaterial) {
				continue;
			}

			// 基本色の読み取りを試みる
			if (sscanf(line.data(), " Kd %f %f %f",
				&pMaterial->baseColor.x, &pMaterial->baseColor.y,
				&pMaterial->baseColor.z) == 3) {
				continue;
			}

			// 不透明度の読み取りを試みる
			if (sscanf(line.data(), " d %f", &pMaterial->baseColor.w) == 1) {
				continue;
			}

		
			// 基本色テクスチャ名の読み取りを試みる
			char textureDiffuse[1000] = { 0 };
			if (sscanf(line.data(), " map_Kd %999[^\n]", &textureDiffuse) == 1) {
				pMaterial->texDiffuse =
					Texture::Create(foldername + textureDiffuse, GL_LINEAR, Texture::Usage::for3D);
				continue;
			}

			// ノーマルテキスチャ名の読み取りを試みる
			char textureNormal[1000] = {0};
			if (sscanf(line.data(), " map_Bump %999[^\n]", &textureNormal) == 1) {
				pMaterial->texNormal =
					Texture::Create(foldername + textureNormal, GL_LINEAR, Texture::Usage::for3D);
				continue;
			}

			// 法線テキスチャ名の読み取りを試みる
			char textureSpecular[1000] = { 0 };
			if (sscanf(line.data(), " map_Ks %999[^\n]", &textureSpecular) == 1) {
				pMaterial->texSpeculer =
					Texture::Create(foldername + textureSpecular, GL_LINEAR, Texture::Usage::for3D);
				continue;
			}
		}

		// 読み込んだマテリアルの配列を返す
		return materials;
	}


	PrimitiveBuffer::PrimitiveBuffer(size_t vboCapacity, size_t iboCapacity)
	{
		const GLenum flags = GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

		// VBOを作成
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vboCapacity, nullptr, flags);
		pVbo = static_cast<uint8_t*>(glMapNamedBufferRange(vbo, 0, vboCapacity, flags));

		// IBOを作成
		glCreateBuffers(1, &ibo);
		glNamedBufferStorage(ibo, iboCapacity, nullptr, flags);
		pIbo = static_cast<uint8_t*>(glMapNamedBufferRange(ibo, 0, iboCapacity, flags));

		// VAOを作成し、OpenGLコンテキストに割り当てる
		vao = VertexArray::Create();
		glBindVertexArray(*vao);

		// VAOに、VBOとIBOを割り当てる
		// 頂点アトリビュート設定前に行う必要がある
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// 頂点アトリビュートを設定
		const auto setAttribute = [](GLuint index, GLint size, size_t offset) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				reinterpret_cast<void*>(offset));
		};
		setAttribute(0, 3, offsetof(Vertex, position));
		setAttribute(1, 2, offsetof(Vertex, texcoord));
		setAttribute(2, 3, offsetof(Vertex, normal));

		// OpenGLコンテキストへの割り当てを解除
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	PrimitiveBuffer::~PrimitiveBuffer()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	void PrimitiveBuffer::AddPrimitive(const Vertex* vertices, size_t vertexCount, const uint16_t* indices, size_t indexCount, GLenum mode)
	{
		Primitive prim;
		prim.mode = mode;
		prim.count = static_cast<GLsizei>(indexCount);
		prim.indices = AddIndices(indices, indexCount);
		prim.baseVertex = AddVertices(vertices, vertexCount);
		primitives.push_back(prim);
	}

	GLint PrimitiveBuffer::AddVertices(const Vertex* vertices, size_t count)
	{
		const GLint offset = static_cast<GLint>(vboSize / sizeof(Vertex));

		const size_t verticesSize = count * sizeof(Vertex);
		memcpy(pVbo + vboSize, vertices, verticesSize);
		vboSize += verticesSize;

		return offset;
	}

	const void* PrimitiveBuffer::AddIndices(const uint16_t* indices, size_t count)
	{
		const void* offset = reinterpret_cast<void*>(iboSize);

		const size_t indexSize = count * sizeof(uint16_t);
		memcpy(pIbo + iboSize, indices, indexSize);
		iboSize += indexSize;

		return offset;
	}

	void PrimitiveBuffer::Clear()
	{
		primitives.clear();
		vboSize = 0;
		iboSize = 0;
	}

	MeshBuffer::MeshBuffer(size_t vboCapacity, size_t iboCapacity) :
		primitiveBuffer(vboCapacity, iboCapacity)
	{}

	StaticMeshPtr MeshBuffer::LoadOBJ(const char* filename)
	{
		// 以前に読み込んだファイルなら、作成済みのメッシュを返す
		{
			auto itr = meshes.find(filename);
			if (itr != meshes.end()) {
				return itr->second;
			}
		}

		// OBJファイルを開く
		std::ifstream file(filename);
		if (!file) {
			LOG_ERROR("%sを開けません", filename);
			return nullptr;
		}

		// フォルダ名を取得する
		std::string foldername(filename);
		{
			const size_t p = foldername.find_last_of("\\/");
			if (p != std::string::npos) {
				foldername.resize(p + 1);
			}
		}

		// OBJファイルを解析して、頂点データとインデックスデータを読み込む
		struct IndexSet { int v, vt, vn; };
		struct UseMaterial {
			std::string name;   // マテリアル名
			size_t startOffset; // 割り当て範囲の先頭位置
		};

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texcoords;
		std::vector<glm::vec3> normals;
		std::vector<IndexSet> faceIndexSet;
		std::vector<MaterialPtr> materials;
		std::vector<UseMaterial> usemtls;

		positions.reserve(20'000);
		texcoords.reserve(20'000);
		normals.reserve(20'000);
		faceIndexSet.reserve(20'000 * 3);
		materials.reserve(100);
		usemtls.reserve(100);

		// 仮データを追加(マテリアル指定がないファイル対策)
		usemtls.push_back({ std::string(), 0 });

		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			const char* p = line.c_str();

			// 頂点座標の読み取りを試みる
			glm::vec3 v;
			if (sscanf(p, " v %f %f %f", &v.x, &v.y, &v.z) == 3) {
				positions.push_back(v);
				continue;

			}

			// テクスチャ座標の読み取りを試みる
			glm::vec2 vt;
			if (sscanf(p, " vt %f %f", &vt.x, &vt.y) == 2) {
				texcoords.push_back(vt);
				continue;

			}

			// 法線の読み取りを試みる
			glm::vec3 vn;
			if (sscanf(p, " vn %f %f %f", &vn.x, &vn.y, &vn.z) == 3) {
				normals.push_back(vn);
				continue;
			}

			// インデックスデータの読み取りを試みる
			IndexSet f0, f1, f2;
			int readByte;

			// 頂点座標+テクスチャ座標+法線
			if (sscanf(p, " f %u/%u/%u %u/%u/%u%n",
				&f0.v, &f0.vt, &f0.vn, &f1.v, &f1.vt, &f1.vn, &readByte) == 6) {

				p += readByte; // 読み取り位置を更新
				for (;;) {
					if (sscanf(p, " %u/%u/%u%n", &f2.v, &f2.vt, &f2.vn, &readByte) != 3) {
						break;
					}
					p += readByte; // 読み取り位置を更新
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);
					f1 = f2; // 次の三角形のためにデータを移動		
				}
				continue;
			}

			// 頂点座標+テクスチャ座標
			if (sscanf(p, " f %u/%u %u/%u%n",
				&f0.v, &f0.vt, &f1.v, &f1.vt, &readByte) == 4) {
				f0.vn = f1.vn = 0; // 法線なし
				p += readByte; // 読み取り位置を更新
				for (;;) {
					if (sscanf(p, " %u/%u%n", &f2.v, &f2.vt, &readByte) != 2) {
						break;
					}
					f2.vn = 0; // 法線なし
					p += readByte; // 読み取り位置を更新
					faceIndexSet.push_back(f0);
					faceIndexSet.push_back(f1);
					faceIndexSet.push_back(f2);
					f1 = f2; // 次の三角形のためにデータを移動

				}
				continue;
			}

			// MTLファイルの読み取りを試みる
			char mtlFilename[1000];
			if (sscanf(line.data(), " mtllib %999s", mtlFilename) == 1) {
				const auto tmp = LoadMTL(foldername, mtlFilename);
				materials.insert(materials.end(), tmp.begin(), tmp.end());
				continue;
			}

			// 使用マテリアル名の読み取りを試みる
			char mtlName[1000];
			if (sscanf(line.data(), " usemtl %999s", mtlName) == 1) {
				// 前回と異なるマテリアルが指定された場合のみ、新しいマテリアル範囲を追加
				if (usemtls.back().name != mtlName) {
					usemtls.push_back({ mtlName, faceIndexSet.size() });
				}
				continue;
			}
		}

		// 末尾に番兵を追加
		usemtls.push_back({ std::string(), faceIndexSet.size() });

		// 読み込んだデータを、OpenGLで使えるデータに変換
		std::vector<Vertex> vertices;
		vertices.reserve(faceIndexSet.size());
		std::vector<uint16_t> indices;
		indices.reserve(faceIndexSet.size());
		std::unordered_map<uint64_t, uint16_t> vertexMap;
		vertexMap.reserve(faceIndexSet.size());


		for (const auto& e : faceIndexSet) {
			// 頂点IDを作成
			const uint64_t vertexId =
				(static_cast<uint64_t>(e.v) << 42) +
				(static_cast<uint64_t>(e.vt) << 21) +
				static_cast<uint64_t>(e.vn);

			// 一致する頂点IDがあれば、そのペアのインデックスを使う
			const auto itr = vertexMap.find(vertexId);
			if (itr != vertexMap.end()) {
				indices.push_back(itr->second);
				continue;
			}

			// 一致する頂点IDがなければ、新しいインデックスと頂点データを作成する
			const uint16_t index = static_cast<uint16_t>(vertices.size());
			indices.push_back(index);
			vertexMap.emplace(vertexId, index); // 頂点IDとインデックスのペアを追加


			// 新しい頂点データを作成
			Vertex v;
			v.position = positions[e.v - 1];
			v.texcoord = texcoords[e.vt - 1];
			// 法線がない場合は仮データを設定
			if (e.vn == 0) {
				v.normal = glm::vec3(0, 0, 1);
			}
			else {
				v.normal = normals[e.vn - 1];
			}
			vertices.push_back(v);

		}



		// 変換したデータをプリミティブバッファに追加
		const GLint baseVertex =
			primitiveBuffer.AddVertices(vertices.data(), vertices.size());
		const void* indexOffset =
			primitiveBuffer.AddIndices(indices.data(), indices.size());

		//メッシュを作成
		auto pMesh = std::make_shared<StaticMesh>();
		pMesh->name = filename;


		// プリミティブを作成
		size_t i = 0;
		if (usemtls.size() > 2) {
			i = 1; // 仮データと番兵以外のマテリアルがある場合、仮データを飛ばす
		}
		for (; i < usemtls.size() - 1; ++i) {
			const UseMaterial& cur = usemtls[i];
			const UseMaterial& next = usemtls[i + 1];

			if (next.startOffset == cur.startOffset) {
				continue; // インデックスデータがない場合は飛ばす
			}


			Primitive prim;
			prim.mode = GL_TRIANGLES;
			prim.count = static_cast<GLsizei>(next.startOffset - cur.startOffset);
			prim.indices = indexOffset;
			prim.baseVertex = baseVertex;

			prim.materialNo = 0; // デフォルト値を設定
			for (int i = 0; i < materials.size(); ++i) {
				if (materials[i]->name == cur.name) {
					prim.materialNo = i; // 名前の一致するマテリアルを設定
					break;
				}
			}
			pMesh->primitives.push_back(prim);

			// インデックスオフセットを進める
			indexOffset = reinterpret_cast<void*>(
				reinterpret_cast<size_t>(indexOffset) + sizeof(uint16_t) * prim.count);
		}

		// マテリアル配列が空の場合、デフォルトマテリアルを追加
		if (materials.empty()) {
			pMesh->materials.push_back(std::make_shared<Material>());
		}
		else {
			pMesh->materials.assign(materials.begin(), materials.end());
		}

		meshes.emplace(pMesh->name, pMesh);

		//LOG("%sを読み込みました", filename);

		return pMesh;
	}

	VertexArrayPtr MeshBuffer::GetVAO() const
	{
		return primitiveBuffer.GetVAO();;
	}

	void MeshBuffer::Clear()
	{
		meshes.clear();
		primitiveBuffer.Clear();
	}



}