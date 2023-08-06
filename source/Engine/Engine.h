#pragma once
#include "Window/GLFWWindow.h"
#include <string>
#include <vector>
#include <memory>
#include "BufferObject/FrameBufferObject.h"
#include "BufferObject/BufferObject.h"
#include "BufferObject/VertexArray.h"
#include "BufferObject/CameraBufferObject.h"
#include "BufferObject/AnimBuffer.h"
#include "Time/StopAndSlowry.h"
#include "2DImage/Texture.h"
#include "2DImage/Sprite.h"
#include "Std3D/StdMesh.h"
#include "Scene/Scene.h"
#include "Camera.h"
#include "Model.h"
#include "Animation.h"
#include "DataStruct/InstanceModelDataList.h"
#include "DataStruct/AnimUseList.h"
#include "GameObject/Actor/LoadingGauge.h"

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;
using ShaderList = std::vector<ShaderPtr>;
using TextureList = std::vector<TexturePtr>;
using AnimationPtr = std::shared_ptr<Animation>;
using AnimationList = std::vector<AnimationPtr>;
class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;
using GameObjectList = std::vector<GameObjectPtr>;

/*使用したいリソースの作成はエンジンで行う*/
class Engine
{
private:
	std::string projectName = "FantasyWizard";
public:
	static Engine& Instance()
	{
		static Engine instance;
		return instance;
	}
	/*ランタイム関数*/
	int Run();

	/*テクスチャ関連*/
	TexturePtr CreateTexture(std::string name, GLenum filterMode = GL_NEAREST, Texture::Usage usage = Texture::Usage::for2D);
	TexturePtr GetTexture(std::string name);
	TextureList GetTextures();

	/*3Dモデル関連*/
	std::shared_ptr<Model> LoadModel(std::string name, std::string path);
	std::shared_ptr<Model> GetModel(std::string name);

	/*アニメーション関連*/
	AnimationPtr CreateAnimation(const char* path, std::shared_ptr<Model> model, bool loop = true);
	AnimationPtr GetAnimation(unsigned int id);
	AnimationPtr CreateAnimation(const char* path, std::string model, bool loop = true);

	/*アニメーションデータ管理*/
	std::shared_ptr<AnimUseList> GetAnimUseList(std::string name);
	void SetAnimUseList(std::shared_ptr<AnimUseList>& dataList);

	/*ストップ&スロウ関連*/
	inline void SetSlowlyTime(float time) {
		slowryManager.SetSlowTime(time);
	}
	inline void SetSlowPace(int pace) {
		slowryManager.SetSlowPace(pace);
	}
	inline void SetStopTime(float time)noexcept {
		stopManager.SetStopTime(time);
	}
	inline void StopSwitch() {
		stopManager.StopSwitch();
	}

	/*インスタンス描画用データのセッター*/
	void SetAnimData(int materialID, Mesh::Mesh& mesh, glm::mat4& model, std::vector <glm::mat4>& transforms) {
		instanceData.SetAnimData(materialID, mesh, model, transforms);
	}
	void SetMeshData(int materialID, Mesh::Mesh& mesh, glm::mat4& model) {
		instanceData.SetMeshData(materialID, mesh, model);
	}

	/*シーン推移*/
	template<typename T>
	void SetNextScene() { nextScene = std::make_shared<T>(); }

	/*デバッグ*/
	void Debug()noexcept {
		stopManager.Debug();
		slowryManager.Debug();

		ImGui::Begin("Assets");
		if (!shaders.empty()) {
			ImGui::Text("Shader");
			for (auto itr : shaders) {
				ImGui::Text(itr->name.c_str());
			}
		}
		if (!models.empty()) {
			ImGui::Separator();
			ImGui::Text("Model");
			for (auto itr : models) {
				ImGui::Text(itr.first.c_str());
			}
		}
		if (!animationList.empty()) {
			ImGui::Separator();
			ImGui::Text("Animation");
			for (auto itr : animationList) {
				ImGui::Text(itr->GetName().c_str());
			}
		}
		if (!charaAnimData.empty()) {
			ImGui::Separator();
			ImGui::Text("CharaSetAnimData");
			for (auto& itr : charaAnimData) {
				ImGui::Text(itr->GetName().c_str());
			}
		}
		if (!textures.empty()) {
			ImGui::Separator();
			ImGui::Text("Texture");
			for (auto itr : textures) {
				ImGui::Text(itr->GetName().c_str());
			}
		}
		if (scene.get() != nullptr) {
			ImGui::Separator();
			scene->Debug();
		}
		ImGui::End();

		ImGui::Begin("FrameEffect");
		ImGui::DragFloat("blurStrength", &blurStrength);
		ImGui::DragFloat("bloomThreshold", &bloomThreshold);
		ImGui::DragFloat("bloomStrength", &bloomStrength);
		ImGui::End();
	}

	enum class	 FadeState {
		fading, // フェードイン・フェードアウト実行中
		open,   // フェードイン完了
		closed, // フェードアウト完了
		fadeout,
		fadein,
	};

	void StartFadeIn(float time = defaultFadeTime);
	void StartFadeOut(float time = defaultFadeTime);
	FadeState GetFadeState() const { return fadeState; }
private:

	Engine()noexcept {};
	~Engine()noexcept = default;
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	/*シェーダ関連*/
	ShaderPtr CreateShader(std::string name, const char* vert, const char* frag, const char* geo = "");
	ShaderPtr GetShader(std::string name);
	/*フレームバッファ関連*/
	
	/*ランタイム関数*/
	int Initialize();
	int MainLoop();
	int Finalize();

	/*描画関数*/
	void DrawSkyBox();
	void Draw3D(ShaderPtr shaderMesh, ShaderPtr shaderAnim, bool shadow = false);
	void DrawUI();
	void DrawBloomEffect();
	void DrawFadeEffect();
	void DrawGameWindow(glm::vec2 viewSize);
	void DrawCollision();
	
	void DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset);
	void DrawMeshs(ShaderPtr shader,bool shadow = false);
	void DrawAnims(ShaderPtr shader,bool shadow = false);
	glm::mat4 CreateDepthMap();

	/*SSBOにバインディング*/
	void BindCamera();
	void MemcpyObject();
	void MemcpySprites();
	void UnbindCamera();

	void BindSSBO();
	void UnbindSSBO();

	void CreateVAO(float x, float y, float ax, float ay, BufferObjectPtr& vbo, BufferObjectPtr& ibo, VertexArrayPtr& vao);

	//頂点バッファ
	BufferObjectPtr vbo = nullptr; /*頂点データをバーテックスバッファにコピー*/
	BufferObjectPtr ibo = nullptr; /*インデックスバッファの管理番号*/
	VertexArrayPtr  vao = nullptr; /*構成情報の管理番号*/
	
	BufferObjectPtr gaugeVbo = nullptr; /*頂点データをバーテックスバッファにコピー*/
	BufferObjectPtr gaugeIbo = nullptr; /*インデックスバッファの管理番号*/
	VertexArrayPtr  gaugeVao = nullptr; /*構成情報の管理番号*/

	FramebufferObjectPtr fboGameWindow;  // ゲームウィンドウ用FBO
	FramebufferObjectPtr fboShadow;
	FramebufferObjectPtr fboBloomList[6];
	FramebufferObjectPtr fboFade;

	ShaderList shaders = {};
	SpriteList spriteListPostProcess;
	SpriteList fadePostProcess;
	AnimationList animationList = {};

	TextureList textures = {};

	ScenePtr scene = nullptr;
	ScenePtr nextScene = nullptr;

	std::vector<std::shared_ptr<AnimUseList>> charaAnimData = {};

	std::map<std::string, std::shared_ptr<Model>> models = {};

	StdMesh::MeshBufferPtr meshBuffer;

	ShaderPtr fadeShader{};
	TexturePtr texFadeColor{};
	TexturePtr texFadeRule;
	float fadeFrom = 0;          // シーン遷移エフェクト開始時のしきい値
	float fadeTo = 1;            // シーン遷移エフェクトと終了時のしきい値
	float fadeTotalTime = 1;     // シーン遷移エフェクトにかかる時間
	float fadeTimer = 0;         // シーン遷移エフェクトの経過時間
	FadeState fadeState = FadeState::open; // エフェクトの状態

	std::shared_ptr<LoadingGauge> loadGauge;

	// シーン遷移エフェクトのデフォルト動作時間
	static constexpr float defaultFadeTime = 1.5f;

	float bloomThreshold = 1.5f; //高輝度　-はダメ
	float bloomStrength = 4.f;  //ブルームの強さ
	float blurStrength = 0.3f;		// ぼかし強度(0.0～1.0)

	MappedBufferObjectPtr ssboSprite = {};
	MappedBufferObjectPtr ssboAnim = {};
	MappedBufferObjectPtr ssboMesh = {};
	MappedBufferObjectPtr ssboCamera = {};
	InstanceModelDataList instanceData = {};

	size_t ssboOffset = 0;
	
	Stop stopManager = {};
	Slowry slowryManager = {};

	struct CameraData {
		glm::vec3 position = {};
		//float dammy = 0;
		glm::mat4 projection = {};
		glm::mat4 view = {};
	};
	CameraData cameraData = {};
};