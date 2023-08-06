#include "Collider.h"
#include "GameObject/GameObject.h"

namespace /* unnamed */ {

	/**
	* コライダー型に対応する衝突判定関数を呼び出す
	*/
	template<typename T, typename U>
	bool Func(const Collider& a, const Collider& b,
		ContactPoint& cp)
	{
		return Intersect(static_cast<const T&>(a).GetShape(),
			static_cast<const U&>(b).GetShape(), cp);
	}

	bool NotCollide(const Collider& a, const Collider& b,
		ContactPoint& cp)
	{

		return false;
	}
	/*
	* Object,
	* Player,
	* Enemy,
	* PlAttack,
	* EnAttack,
	* TalkField,
	*/
	bool TagList[static_cast<int>(Collider::Tag::Max)]
		[static_cast<int>(Collider::Tag::Max)]{
			{false,true,true,false,false,false },//Object
			{true,false,true,false,true,true}, //Player
			{true,true,true,true,false,false},  //Enemy
			{false,false,true,false,false,false},//PlAttack
			{false,true,false,false,false,false},//EnAttack
			{false,true,false,false,false,false},//TalkField
	};

} // unnamed namespace


void SphereCollider::AddPosition(const glm::vec3& translate)
{
	sphere.center += translate;
}

ColliderPtr SphereCollider::GetTransformedCollider(const glm::vec3& translation, const glm::mat3& matRotation, const glm::vec3& scale)
{
	auto world = std::make_shared<SphereCollider>();
	world->sphere.center = translation + matRotation * (sphere.center * scale);
	world->sphere.radius = sphere.radius * (std::max)({ scale.x, scale.y, scale.z });
	return world;
}

ColliderPtr SphereCollider::GetTransformedCollider(const Transform& transform)
{
	auto scale = transform.getLocalScale();
	auto world = std::make_shared<SphereCollider>();
	world->sphere.center = transform.getLocalPosition() +
		transform.getRotationMat() * (sphere.center * scale);
	world->sphere.radius = sphere.radius * (std::max)({ scale.x, scale.y, scale.z });
	return world;
}

void SphereCollider::DebugDraw(ShaderPtr& shader, const glm::mat4& model)
{
}

void SphereCollider::SetUp()
{
}

void AABBCollider::AddPosition(const glm::vec3& translate)
{
	aabb.center += translate;
}

ColliderPtr AABBCollider::GetTransformedCollider(const glm::vec3& translation, const glm::mat3& matRotation, const glm::vec3& scale)
{
	auto world = std::make_shared<AABBCollider>();
	world->aabb.extent = aabb.extent;
	world->aabb.center = translation + matRotation * (aabb.center * scale);
	return world;
}

ColliderPtr AABBCollider::GetTransformedCollider(const Transform& transform)
{
	auto world = std::make_shared<AABBCollider>();
	world->aabb.extent = aabb.extent * transform.getLocalScale();
	world->aabb.center = transform.getLocalPosition() +
		transform.getRotationMat() * (aabb.center * transform.getLocalScale());
	return world;
}

void AABBCollider::DebugDraw(ShaderPtr& shader, const glm::mat4& model)
{
	drawCollision.Draw(shader, model, color);
}

void AABBCollider::SetUp()
{
	//drawCollision.SetUp(aabb.getVertice(), aabb.getIndices());
}

bool Intersect(const Collider& a, const Collider& b, ContactPoint& cp)
{
	if (!TagList
		[static_cast<int>(a.GetTag())]
	[static_cast<int>(b.GetTag())])return false;

	// 関数ポインタ型を定義
	using FuncType =
		bool(*)(const Collider&, const Collider&, ContactPoint&);

	// 関数テーブル
	static const FuncType funcList[4][4] = {
	  {
		Func<SphereCollider, SphereCollider>,
		Func<SphereCollider, AABBCollider>,
		Func<SphereCollider, BoxCollider>,
		Func<SphereCollider, CapsuleCollider>,
	  }, {
		Func<AABBCollider, SphereCollider>,
		Func<AABBCollider, AABBCollider>,
		Func<AABBCollider, BoxCollider>,
		Func<AABBCollider, CapsuleCollider>,
	}, {
		Func<BoxCollider, SphereCollider>,
		Func<BoxCollider, AABBCollider>,
		Func<BoxCollider, BoxCollider>,
		Func<BoxCollider, CapsuleCollider>,	
	  },{
		Func<CapsuleCollider, SphereCollider>,
		Func<CapsuleCollider, AABBCollider>,
		Func<CapsuleCollider, BoxCollider>,
		Func<CapsuleCollider, CapsuleCollider>,
		}
	};

	// コライダーの型に対応する衝突判定関数を実行
	const int shapeA = static_cast<int>(a.GetType());
	const int shapeB = static_cast<int>(b.GetType());
	return funcList[shapeA][shapeB](a, b, cp);
}

void WorldCollider::AddPosition(const glm::vec3& v)
{
	gameObject->AddPosition(v);
	for (auto& element : colliderList) {
		element->AddPosition(v);
	}
}


std::vector<Collider::Tag> WorldCollider::DetectCollision(WorldCollider& other)
{
	std::vector<Collider::Tag> tagList{};
	for (auto& ca : colliderList) {
		for (auto& cb : other.colliderList) {
			ContactPoint cp;
			if (!Intersect(*ca, *cb, cp)) {
				continue; // 衝突していない
			}

			// 衝突した
			tagList.push_back(ca->GetTag());
			tagList.push_back(cb->GetTag());

			if (ca->GetOnceCheck()) {
				ca->OnceCollide(true);
			}
			if (cb->GetOnceCheck()) {
				cb->OnceCollide(true);
			}

			// 座標を戻す
			if (ca->IsTrriger()|| cb->IsTrriger()){
				continue;
			}		

			const glm::vec3 v = cp.normal * cp.penetration * 0.5f;
			if (gameObject->rigid == GameObject::Rigid::Dynamic)
				AddPosition(-v);
			if (other.gameObject->rigid == GameObject::Rigid::Dynamic)
				other.AddPosition(v);
		} // for cb
	} // for ca

	return tagList;
}

void BoxCollider::AddPosition(const glm::vec3& translate)
{
	box.center += translate;
}

ColliderPtr BoxCollider::GetTransformedCollider(const glm::vec3& translation, const glm::mat3& matRotation, const glm::vec3& scale)
{
	auto world = std::make_shared<BoxCollider>();
	world->box.center = translation + matRotation * (box.center * scale);
	for (int i = 0; i < 3; ++i) {
		world->box.axis[i] = matRotation * box.axis[i];
	}
	world->box.extent = box.extent * scale;
	return world;
}

ColliderPtr BoxCollider::GetTransformedCollider(const Transform& transform)
{
	auto world = std::make_shared<BoxCollider>();

	auto matRotation = transform.getRotationMat();
	world->box.center = transform.getLocalPosition() +
		matRotation * (box.center * transform.getLocalScale());

	for (int i = 0; i < 3; ++i) {
		world->box.axis[i] = matRotation * box.axis[i];
	}
	world->box.extent = box.extent * transform.getLocalScale();
	return world;
}

void BoxCollider::DebugDraw(ShaderPtr& shader, const glm::mat4& model)
{
}

void BoxCollider::SetUp()
{
}

void CapsuleCollider::AddPosition(const glm::vec3& translate)
{
	capsule.seg.start += translate;
	capsule.seg.end += translate;
}

ColliderPtr CapsuleCollider::GetTransformedCollider(const glm::vec3& translation, const glm::mat3& matRotation, const glm::vec3& scale)
{
	auto world = std::make_shared<CapsuleCollider>();
	world->capsule.seg.start =
		translation + matRotation * (capsule.seg.start * scale);
	world->capsule.seg.end =
		translation + matRotation * (capsule.seg.end * scale);
	// 半径のスケールにはXYZのうち最大のものを選ぶ(Unityの挙動に準拠)
	world->capsule.radius =
		capsule.radius * (std::max)({ scale.x, scale.y, scale.z });
	return world;
}

ColliderPtr CapsuleCollider::GetTransformedCollider(const Transform& transform)
{
	auto world = std::make_shared<CapsuleCollider>();
	auto scale = transform.getLocalScale();
	auto translation = transform.getLocalPosition();
	auto matRotation = transform.getRotationMat();
	world->capsule.seg.start =
		translation + matRotation * (capsule.seg.start * scale);
	world->capsule.seg.end =
		translation + matRotation * (capsule.seg.end * scale);
	// 半径のスケールにはXYZのうち最大のものを選ぶ(Unityの挙動に準拠)
	world->capsule.radius =
		capsule.radius * (std::max)({ scale.x, scale.y, scale.z });
	return world;
}

void CapsuleCollider::DebugDraw(ShaderPtr& shader, const glm::mat4& model)
{
}

void CapsuleCollider::SetUp()
{
}
