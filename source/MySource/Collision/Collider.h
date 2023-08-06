#pragma once
#include "Collision/Collision3D.h"
#include "DrawCollision.h"
#include "Time/Time.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <memory>

// 先行宣言
class Collider;
using ColliderPtr = std::shared_ptr<Collider>;

/**
* 衝突形状
*/
class Collider
{
public:
	Collider() = default;
	virtual ~Collider() = default;
	
	// コライダーの形状
	enum class Type {
		Sphere, // 球体
		AABB,   // 軸平行境界ボックス
		Box,	// 有向境界ボックス	
		Capsule,// カプセル
	};
	//コライダーの設定タグ
	enum class Tag {
		Object,
		Player,
		Enemy,
		PlAttack,
		EnAttack,
		TalkField,
		Max,
	};
	
	virtual void SetTimer(float time) = 0;
	virtual bool CheckActiveTime() = 0;
	virtual void Execute() = 0;
	virtual Type GetType() const = 0;
	virtual Tag GetTag()const = 0;
	virtual void SetTag(Tag tag) = 0;
	virtual bool IsTrriger()const = 0;
	virtual void IsTrriger(bool value) = 0;
	virtual bool OnceCollide() = 0;
	virtual void OnceCollide(bool value) = 0;
	virtual void SetOnceCheck(bool value) = 0;
	virtual bool GetOnceCheck() = 0;
	// 座標を変更する
	virtual void AddPosition(const glm::vec3& translate) = 0;

	// 座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const glm::vec3& translation,
		const glm::mat3& matRotation,
		const glm::vec3& scale) = 0;

	virtual ColliderPtr GetTransformedCollider(const Transform& transform) = 0;

	virtual void DebugDraw(ShaderPtr& shader, const glm::mat4& model) = 0;
	virtual void SetUp() = 0;
};

class SphereCollider;
using SphereColliderPtr = std::shared_ptr<SphereCollider>;

class SphereCollider
	: public Collider
{
public:
	SphereCollider() = default;
	virtual ~SphereCollider() = default;

	// コライダーの形状
	virtual Type GetType() const override { return Type::Sphere; }
	const Sphere& GetShape() const { return sphere; }
	virtual bool IsTrriger()const override {
		return isTrriger;
	}
	virtual void IsTrriger(bool value)override {
		isTrriger = value;
	}
	virtual Tag GetTag()const override {
		return tag;
	}
	virtual void SetTag(Tag tag)override {
		this->tag = tag;
	}
	// 座標を変更する
	virtual void AddPosition(const glm::vec3& translate) override;
	// 座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const glm::vec3& translation,
		const glm::mat3& matRotation,
		const glm::vec3& scale) override;
	virtual ColliderPtr GetTransformedCollider(const Transform& transform)override;
	virtual void DebugDraw(ShaderPtr& shader, const glm::mat4& model)override;
	virtual void SetUp()override;
	virtual void SetTimer(float time)override {
		timer = time;
	}
	virtual bool CheckActiveTime() {
		if (timer == 0.f)return false;
		if (timer <= increass) return true;
		return false;
	};
	virtual void Execute()override {
		if (timer != 0.f){
			increass += Time::deltaTime;
		}	
	};
	virtual bool OnceCollide() {
		return oneCollideFlag;
	};
	virtual void OnceCollide(bool value) {
		oneCollideFlag = value;
	}
	virtual void SetOnceCheck(bool value) {
		oneCollide = value;
	};
	virtual bool GetOnceCheck()
	{
		return oneCollide;
	}
	bool oneCollide = false;
	bool oneCollideFlag = false;

	float timer = 0.f;
	float increass = 0.f;

	Sphere sphere{};
	Tag tag{};
	bool isTrriger = false;
	glm::vec4 color{};
};

class AABBCollider : public Collider
{
public:
	AABBCollider() = default;
	virtual ~AABBCollider() = default;

	// コライダーの形状
	virtual Type GetType() const override { return Type::AABB; }
	const AABB& GetShape() const { return aabb; }
	virtual bool IsTrriger()const override {
		return isTrriger;
	}
	virtual void IsTrriger(bool value)override {
		isTrriger = value;
	}
	virtual Tag GetTag()const override {
		return tag;
	}
	virtual void SetTag(Tag tag)override {
		this->tag = tag;
	}
	// 座標を変更する
	virtual void AddPosition(const glm::vec3& translate) override;

	// 座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const glm::vec3& translation,
		const glm::mat3& matRotation,
		const glm::vec3& scale) override;
	virtual ColliderPtr GetTransformedCollider(const Transform& transform)override;
	virtual void DebugDraw(ShaderPtr& shader, const glm::mat4& model)override;
	virtual void SetUp()override;
	virtual void SetTimer(float time)override {
		timer = time;
	}
	virtual bool CheckActiveTime() {
		if (timer == 0.f)return false;
		if (timer <= increass) return true;
		return false;
	};
	virtual void Execute()override {
		if (timer != 0.f) {
			increass += Time::deltaTime;
		}
	};

	virtual bool OnceCollide() {
		return oneCollideFlag;
	};
	virtual void OnceCollide(bool value) {
		oneCollideFlag = value;
	}
	virtual void SetOnceCheck(bool value) {
		oneCollide = value;
	};
	virtual bool GetOnceCheck()
	{
		return oneCollide;
	}
	bool oneCollide = false;
	bool oneCollideFlag = false;
	float timer = 0.f;
	float increass = 0.f;
	AABB aabb{};
	Tag tag{};
	bool isTrriger = false;
	VisualCollision drawCollision{};
	glm::vec4 color{};
};

class BoxCollider : public Collider
{
public:
	BoxCollider() = default;
	virtual ~BoxCollider() = default;

	// 図形を取得する
	virtual Type GetType() const override { return Type::Box; }
	const Box& GetShape() const { return box; }
	virtual bool IsTrriger()const override {
		return isTrriger;
	}
	virtual void IsTrriger(bool value)override {
		isTrriger = value;
	}
	virtual Tag GetTag()const override {
		return tag;
	}
	virtual void SetTag(Tag tag)override {
		this->tag = tag;
	}
	// 座標を変更する
	virtual void AddPosition(const glm::vec3& translate) override;

	// 座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const glm::vec3& translation,
		const glm::mat3& matRotation,
		const glm::vec3& scale) override;
	virtual ColliderPtr GetTransformedCollider(const Transform& transform)override;
	virtual void DebugDraw(ShaderPtr& shader, const glm::mat4& model)override;
	virtual void SetUp()override;
	virtual void SetTimer(float time)override {
		timer = time;
	}
	virtual bool CheckActiveTime() {
		if (timer == 0.f)return false;
		if (timer <= increass) return true;
		return false;
	};
	virtual void Execute()override {
		if (timer != 0.f) {
			increass += Time::deltaTime;
		}
	};
	virtual bool OnceCollide() {
		return oneCollideFlag;
	};
	virtual void OnceCollide(bool value) {
		oneCollideFlag = value;
	}
	virtual void SetOnceCheck(bool value) {
		oneCollide = value;
	};
	virtual bool GetOnceCheck()
	{
		return oneCollide;
	}
	bool oneCollide = false;
	bool oneCollideFlag = false;

	float timer = 0.f;
	float increass = 0.f;
	Box box{};
	Tag tag{};
	bool isTrriger = false;
};

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider() = default;
	virtual ~CapsuleCollider() = default;

	// 図形を取得する
	virtual Type GetType() const override { return Type::Capsule; }
	const Capsule& GetShape() const { return capsule; }
	virtual bool IsTrriger()const override {
		return isTrriger;
	}
	virtual void IsTrriger(bool value)override {
		isTrriger = value;
	}
	virtual Tag GetTag()const override {
		return tag;
	}
	virtual void SetTag(Tag tag)override {
		this->tag = tag;
	}
	// 座標を変更する
	virtual void AddPosition(const glm::vec3& translate) override;

	// 座標変換したコライダーを取得する
	virtual ColliderPtr GetTransformedCollider(
		const glm::vec3& translation,
		const glm::mat3& matRotation,
		const glm::vec3& scale) override;
	virtual ColliderPtr GetTransformedCollider(const Transform& transform)override;
	virtual void DebugDraw(ShaderPtr& shader, const glm::mat4& model)override;
	virtual void SetUp()override;
	virtual void SetTimer(float time)override {
		timer = time;
	}
	virtual bool CheckActiveTime() {
		if (timer == 0.f)return false;
		if (timer <= increass) return true;
		return false;
	};
	virtual void Execute()override {
		if (timer != 0.f) {
			increass += Time::deltaTime;
		}
	};
	virtual bool OnceCollide() {
		return oneCollideFlag;
	};
	virtual void OnceCollide(bool value) {
		oneCollideFlag = value;
	}
	virtual void SetOnceCheck(bool value) {
		oneCollide = value;
	};
	virtual bool GetOnceCheck()
	{
		return oneCollide;
	}
	bool oneCollide = false;
	bool oneCollideFlag = false;
	float timer = 0.f;
	float increass = 0.f;
	Tag tag{};
	Capsule capsule{};
	bool isTrriger = false;
};

bool Intersect(const Collider& a, const Collider& b, ContactPoint& cp);

class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;

struct WorldCollider
{
	// ゲームオブジェクトと衝突判定の座標を変更する
	void AddPosition(const glm::vec3& v);

	// 他のワールドコライダーとの衝突判定を行う
	std::vector<Collider::Tag> DetectCollision(WorldCollider& other);

	std::vector<ColliderPtr> colliderList{};
	GameObjectPtr gameObject{}; // コライダーを所有するゲームオブジェクト
};