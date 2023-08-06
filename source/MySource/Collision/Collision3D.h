#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "GameObject/Transform.h"
#include <array>
#include <vector>
#include <memory>

/**
* ����
*/
struct LineSegment
{
	glm::vec3 start{}; // �����̎n�_
	glm::vec3 end{};   // �����̏I�_
	LineSegment() = default;
	LineSegment(glm::vec3 start,glm::vec3 end)
		:start(start),end(end){}
};

/**
* 3D�����`
*/
struct Rectangle
{
	Rectangle(){};
	Rectangle(glm::vec3 center,glm::vec3 norm,glm::vec3 axis1,glm::vec3 axis2,float scale1,float scale2)
		:center(center), normal(norm), axis{ axis1,axis2 }, scale{ scale1,scale2 }
	{}

	glm::vec3 center{};  // �����`�̒��S���W
	glm::vec3 normal{};  // �ʂ̖@��
	glm::vec3 axis[2]{}; // �����`�̎�
	float scale[2]{};        // �������̑傫��
};

/**
* ����
*/
struct Plane
{
	glm::vec3 normal = { 0.0f,1.0f,0.0f };//�P�ʃx�N�g��
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

//������
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
* �Փ˒n�_
*/
struct ContactPoint {
	glm::vec3 position = {}; // �Փˍ��W
	glm::vec3 normal = {};   // �Փ˕���(����B���畨��A�Ɍ���������)
	float penetration = 0.f;      // �ђʋ���(�}�C�i�X�̏ꍇ�͏Փ˂��Ă���)
};

/*
*���N���X
*/
struct BoundingVolume
{
	virtual bool isOnFrustum(const Frustum& camFrustum,
		const Transform& transform) const = 0;

	virtual bool isOnOrForwardPlan(const Plane& plan)const = 0;

	//�e�X�̖ʂɏՓ˂��Ă��邩�̊m�F(�`�ɂ��)
	bool isOnFrustum(const Frustum& camFrustum)const;
};

/*
* ����
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
* ���l�p�`(2D)
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
* ������
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
* OBB�i��]�t�������́j
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
* �J�v�Z��
*/
struct Capsule :public BoundingVolume
{
	LineSegment seg{}; // ���S�̐���
	float radius = 0.f;    // ���a
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
* OBB�̒��_�����Ɏˉe���A�ő�l�ƍŏ��l�����߂�
*
* @param vertices OBB�̒��_�z��
* @param axis     �ˉe��̎��x�N�g��
* @param min      �ŏ��l���i�[����ϐ�
* @param max      �ő�l���i�[����ϐ�
*/
void ProjectBoxToAxis(const glm::vec3* vertices, const glm::vec3& axis,
	float& min, float& max);

/**
* 2��OBB�����x�N�g���Ɏˉe���A�������Ă��鋗�������߂�
*
* @param verticesA  OBB����1�̒��_�z��
* @param verticesB  OBB����2�̒��_�z��
* @param axis       �ˉe��̎��x�N�g��
* @param shouldFlip �Փ˖@���̔��]�̗L��
*
* @return �������Ă��鋗��(�v�Z�̓s���Ń}�C�i�X�l�ɂȂ��Ă��邱�Ƃɒ���)
*/
float GetIntersectionLength(const glm::vec3* verticesA, const glm::vec3* verticesB,
	const glm::vec3& axis, bool& shouldFlip);

std::vector<LineSegment> GetEdges(const glm::vec3* vertices);

/**
* OBB�̖ʂ��擾����
*
* @param box �ʂ����߂�OBB
*
* @return box�̖ʂ̔z��
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