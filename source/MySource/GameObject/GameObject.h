#pragma once
#include <vector>
#include <memory>
#include <string>
#include <type_traits>
#include "DataStruct/Status.h"
#include "2DImage/Texture.h"
#include "2DImage/SpritePriority.h"
#include "Component/Component.h"
#include "ObjectTag.h"
#include "ObjectSprite.h"
#include "Transform.h"
#include "State/IState.h"
#include "Component/Renderer.h"
#include "../Assimp/Model.h"
#include "../Assimp/Mesh.h"
#include "../Assimp/Animator.h"
#include "Collision/Collision3D.h"
#include "DataStruct/AnimUseList.h"
#include "../Engine/Engine.h"
#include "Collision/Collider.h"


class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;//ゲームオブジェクトのポインタ
using GameObjectList = std::vector<GameObjectPtr>;//ゲームオブジェクトのぽんた配列

class MeshRenderer;
using MeshRendererPtr = std::shared_ptr<MeshRenderer>;
class Shader;

class GameObject
	:public std::enable_shared_from_this<GameObject>
{
private:
	//スタートを通ったかどうか
	bool isStarted = false;
	//このゲームオブジェクトを削除するかどうかのフラグ
	bool isDead = false;
	/*取得するためまたはコリジョン計算のタグ*/
	std::string tag = "untag";
	//フラスタムカリング用の変数
	std::unique_ptr<BoundingVolume> frustumShape = nullptr;

protected:
	//画面に映っているかどうかのbool
	bool isActive = false;
	//このオブジェクトの名前
	std::string name;
	//使用するステータスパラメータ
	std::shared_ptr<STATUS::Status> status = nullptr;
	//行動の推移
	std::shared_ptr<STATE::IState>  state = nullptr;
public:
	//衝突判定コリジョンAABB
	enum class Rigid {
		Static,
		Dynamic,
	};
	std::shared_ptr<AABB> aabb = nullptr;
	std::shared_ptr<Sphere> sphere = nullptr;
	std::vector<ColliderPtr> colliderList;
	Rigid rigid = Rigid::Dynamic;

	//アニメーションの位置を加算するかどうか
	bool animPosFlag = false;
	//アニメーション中の位置
	glm::vec3 animPosition{};
	//使用するアニメーションを補完するリスト
	std::shared_ptr<AnimUseList> animUseList = nullptr;
	/*2D画像の表示優先順位(直訳　優先度)*/
	SpritePriority layer = SpritePriority::prioObject1;
	//2D画像の情報
	ObjectSpriteList spriteList = {};
	//前方方向
	glm::vec3 front = { 1.0f,0.0f,0.0f };
	//位置、回転、大きさ
	Transform transform = {};
	//コンポーネントリスト
	ComponentList componentList = {};
	//3Dモデルの情報
	std::vector<std::shared_ptr<Model>> models = {};
	//アニメーションを動かすための物
	std::shared_ptr<Animator> animator = nullptr;
	//使用するマテリアルのid 
	int materialID = 0;
public:
	GameObject() = default;
	virtual ~GameObject()noexcept = default;
	/*Actorみたいに継承させて入れたいコンポーネントを追加する*/
	virtual void ActorStart()noexcept;
	virtual void ActorUpdate()noexcept;
	/*コンポーネント処理の操作*/
	virtual void Start()noexcept;
	virtual void Update()noexcept;
	virtual void OnCollision(GameObject& other, std::vector<Collider::Tag> tagList)noexcept;
	virtual void OnDestroy()noexcept;
	virtual void TakeDamage(GameObject& other, const Damage& damage)noexcept;
	virtual void OutCollision();

	/*コリジョンの追加*/
	template <typename T>
	std::shared_ptr<T> AddCollision() {
		auto collision = std::make_shared<T>();
		colliderList.push_back(collision);
		return collision;
	}

	/*コンポーネントの増減、または取得*/
	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<T> p = std::make_shared<T>(shared_from_this());
		componentList.push_back(p);
		return p;
	}
	template <typename T>
	std::shared_ptr<T> GetConponent()noexcept
	{
		auto p = std::make_shared<T>(shared_from_this());

		for (auto itr : componentList) {
			if (itr->name == p->name)
			{
				p.reset();
				return std::dynamic_pointer_cast<T>(itr);
			}
		}

		return nullptr;
	}
	template <typename T>
	void RemoveConponent()noexcept
	{
		auto p = std::make_shared<T>(shared_from_this());
		for (auto itr : componentList) {
			if (itr->name == p->name)
			{
				componentList.erase(
					std::remove(componentList.begin(), componentList.end(), itr),
					componentList.end());
				p.reset();
				itr.reset();

				break;
			}
		}
	}

	//現状の位置から引数の分動く時のショートカット
	void AddPosition(glm::vec3 addPosition);

	// 描画のデータを送り出す
	void PutDrawData(Frustum& frustum, int& display, bool isInstance = false);

	/*ステータスの作成*/
	void CreateStatus(const char* path, const char* name);

	/*アニメーターの作成*/
	std::shared_ptr<Animator>
		CreateAnimator(Animation* animation = nullptr);

	/*モデルの作成(ここでフラスタムカリング用のコリジョンも作ってる)*/
	std::shared_ptr<Model> CreateModel(std::string name, std::string path);

	/*アニメーターを持っているかどうか*/
	bool HasAnimator()const noexcept;

	/*ステートの変更と新規作成*/
	template <typename T>
	void SetState()noexcept {
		if (state != nullptr) {
			state->Finalize();
			state.reset();
		}
		state = std::make_shared<T>();
		state->Initialize();
		state->giveGameObject(shared_from_this());
	}

	/*スプライトの操作*/
	void AddSpriteTexture(const Texcoord& tc, TexturePtr tex,
		float x = 0, float y = 0, float scale = 1, float rotation = 0, float alpha = 1)noexcept;
	void AddSprite(const Texcoord& tc, TexturePtr tex,
		float x = 0, float y = 0, float scale = 1, float rotation = 0)noexcept;
	void AddSprite(const Texcoord& tc, std::string name, float x, float y, float scale, float rotation) noexcept;
	const std::vector<ObjectSprite>& GetSprite()const { return spriteList; }

	/*ゲッターセッター*/
	inline std::shared_ptr<STATUS::Status>& GetStatus()
	{
		return status;
	}

	inline void SetName(std::string name)noexcept
	{
		this->name = name;
	}
	inline std::string GetName()const noexcept
	{
		return this->name;
	}

	inline bool IsDead()const noexcept
	{
		return this->isDead;
	}
	inline void IsDead(const bool value)noexcept
	{
		this->isDead = value;
	}

	inline void SetTag(std::string tag)noexcept
	{
		this->tag = TAG::CheckforResistation(tag);
	}
	inline std::string GetTag()const noexcept
	{
		return this->tag;
	}

	inline bool IsStarted()const { return isStarted; }
	inline void IsStarted(bool value) { this->isStarted = value; }

	/*デバッグの情報提出*/
	void Debug()noexcept;
};