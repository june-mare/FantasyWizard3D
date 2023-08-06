#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "GameObject/Transform.h"
#include <array>
#include <vector>
#include <memory>

/**
* 線分
*/
struct LineSegment
{
	glm::vec3 start{}; // 線分の始点
	glm::vec3 end{};   // 線分の終点
	LineSegment() = default;
	LineSegment(glm::vec3 start,glm::vec3 end)
		:start(start),end(end){}
};

/**
* 3D長方形
*/
struct Rectangle
{
	Rectangle(){};
	Rectangle(glm::vec3 center,glm::vec3 norm,glm::vec3 axis1,glm::vec3 axis2,float scale1,float scale2)
		:center(center), normal(norm), axis{ axis1,axis2 }, scale{ scale1,scale2 }
	{}

	glm::vec3 center{};  // 長方形の中心座標
	glm::vec3 normal{};  // 面の法線
	glm::vec3 axis[2]{}; // 長方形の軸
	float scale[2]{};        // 軸方向の大きさ
};

/**
* 平面
*/
struct Plane
{
	glm::vec3 normal = { 0.0f,1.0f,0.0f };//単位ベクトル
	float distance = 0.0f;

	Plane() = default;
	Plane(const glm::vec3& P1, const glm::vec3& norm)
		:normal(glm::normalize(norm)), distance(glm::dot(normal, P1))
	{}
	Plane(const glm::vec3& norm, const float distance)
		:normal(norm), distance(distance)
	{}

	float getSignedDistanceToPlan(const glm::vec3& point)const;

};

//視錐台
struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};

/*
* 衝突地点
*/
struct ContactPoint {
	glm::vec3 position = {}; // 衝突座標
	glm::vec3 normal = {};   // 衝突方向(物体Bから物体Aに向かう方向)
	float penetration = 0.f;      // 貫通距離(マイナスの場合は衝突している)
};

/*
*基底クラス
*/
struct BoundingVolume
{
	virtual bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform) const = 0;

	virtual bool isOnOrForwardPlan(const Plane& plan)const = 0;

	//各々の面に衝突しているかの確認(形による)
	bool isOnFrustum(const Frustum& camFrustum)const;
};

/*
* 球体
*/
struct Sphere : public BoundingVolume
{
	glm::vec3 center{ 0.0f, 0.0f, 0.0 };
	float radius{ 0.0f };

	Sphere(const glm::vec3& inCenter, float inRadius)
		:BoundingVolume{}, center{ inCenter }, radius(inRadius)
	{}
	Sphere(){}

	bool isOnOrForwardPlan(const Plane& plan)const final;

	bool isOnFrustum(const Frustum& camFrustum, const Transform& transform)const final;

};

/*
* 正四角形(2D)
*/
struct SquareAABB :
	public BoundingVolume
{
	glm::vec3 center{ 0.0f,0.0f,0.0f };
	float extent{ 0.0f };
	SquareAABB() {};
	SquareAABB(const glm::vec3& inCenter, float inExtent)
		: BoundingVolume{}, center{ inCenter }, extent{ inExtent }
	{}
	virtual ~SquareAABB() = default;

	bool isOnOrForwardPlan(const Plane& plan)const override final;


	bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform)const override final;

};

/*
* 立方体
*/
struct AABB : public BoundingVolume
{
	glm::vec3 center{ 0.0f,0.0f,0.0f };
	glm::vec3 extent{ 0.0f,0.0f,0.0f };
	AABB() {};
	AABB(const glm::vec3& min, const glm::vec3& max)
		:BoundingVolume{}, center{ (max + min) * 0.5f }
		, extent{ max.x - center.x, max.y - center.y, max.z - center.z }
	{}
	AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
		:BoundingVolume{}, center{ inCenter }, extent{ iI,iJ,iK }
	{}
	void operator=(const AABB& aabb) {
		center = aabb.center;
		extent = aabb.extent;
	};

	std::vector<glm::vec3> getVertice()const;

	std::array<unsigned int, 36> getIndices()const;

	bool isOnOrForwardPlan(const Plane& plan)const override final;


	bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform)const override final;
};

/*
* OBB（回転付き立方体）
*/
struct Box :public BoundingVolume
{
	glm::vec3 center{ 0.0f,0.0f,0.0f };
	glm::vec3 extent{ 0.0f,0.0f,0.0f };
	glm::vec3 axis[3]{
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f }
	};
	Box() {};
	Box(const glm::vec3& min, const glm::vec3& max)
		:BoundingVolume{}, center{ (max + min) * 0.5f }
		, extent{ max.x - center.x, max.y - center.y, max.z - center.z }
	{}
	Box(const glm::vec3& inCenter, float iI, float iJ, float iK)
		:BoundingVolume{}, center{ inCenter }, extent{ iI,iJ,iK }
	{}

	std::vector<glm::vec3> getVertice()const;

	std::array<unsigned int, 36> getIndices()const;

	bool isOnOrForwardPlan(const Plane& plan)const override final;


	bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform)const override final;
};

/*
* カプセル
*/
struct Capsule :public BoundingVolume
{
	LineSegment seg{}; // 中心の線分
	float radius = 0.f;    // 半径
	Capsule() = default;
	Capsule(glm::vec3 start, glm::vec3 end, float radius):
		seg(start,end),radius(radius){}

	bool isOnOrForwardPlan(const Plane& plan)const override final;
	bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform)const override final;
};


class Camera;
Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar);

class Model;
AABB generateAABB(const Model& model);
Sphere generateSphereBV(const Model& model);
Capsule generateCapsule(const Model& model);

glm::vec3 ClosestPoint(const AABB& aabb, const glm::vec3& point);
glm::vec3 ClosestPoint(const Box& box, const glm::vec3& p);
glm::vec3 ClosestPoint(const LineSegment& seg, const glm::vec3& point);
LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB);
LineSegment ClosestPoint(const struct Rectangle& rect, const LineSegment& seg);

/*
* OBBの頂点を軸に射影し、最大値と最小値を求める
*
* @param vertices OBBの頂点配列
* @param axis     射影先の軸ベクトル
* @param min      最小値を格納する変数
* @param max      最大値を格納する変数
*/
void ProjectBoxToAxis(const glm::vec3* vertices, const glm::vec3& axis,
	float& min, float& max);

/**
* 2つのOBBを軸ベクトルに射影し、交差している距離を求める
*
* @param verticesA  OBBその1の頂点配列
* @param verticesB  OBBその2の頂点配列
* @param axis       射影先の軸ベクトル
* @param shouldFlip 衝突法線の反転の有無
*
* @return 交差している距離(計算の都合でマイナス値になっていることに注意)
*/
float GetIntersectionLength(const glm::vec3* verticesA, const glm::vec3* verticesB,
	const glm::vec3& axis, bool& shouldFlip);

std::vector<LineSegment> GetEdges(const glm::vec3* vertices);

/**
* OBBの面を取得する
*
* @param box 面を求めるOBB
*
* @return boxの面の配列
*/
std::vector<Plane> GetPlanes(const Box& box);

bool IsPointInsideBox(const glm::vec3& point, const Box& box);

bool ClipLineSegment(const LineSegment& seg, const Plane& plane, glm::vec3& point);
std::vector<glm::vec3> ClipEdges(const std::vector<glm::vec3>& vertices, const Box& box);
float GetIntersectionLength(const Capsule& capsule, const glm::vec3* verticesB, const glm::vec3& axis, bool& shouldFlip, int& minIndex, int& maxIndex);

bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp);
bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp);
bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp);
bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp);
bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp);
bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp);
bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp);
bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp);
bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp);
bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp);
bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp);
bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp);
bool Intersect(const Capsule & capsule, const Box & box, ContactPoint & cp);
bool Intersect(const Box & box, const Capsule & capsule, ContactPoint & cp);
bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp);
bool Intersect(const AABB& aabb, const Capsule& capsule, ContactPoint& cp);