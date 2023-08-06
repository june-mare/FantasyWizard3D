#include "Collision3D.h"
#include "Camera.h"
#include "Model.h"
#include <algorithm>

float Plane::getSignedDistanceToPlan(const glm::vec3& point) const
{
	return glm::dot(normal, point) - distance;
}

bool BoundingVolume::isOnFrustum(const Frustum& camFrustum) const
{
	return (isOnOrForwardPlan(camFrustum.leftFace) &&
		isOnOrForwardPlan(camFrustum.rightFace) &&
		isOnOrForwardPlan(camFrustum.topFace) &&
		isOnOrForwardPlan(camFrustum.bottomFace) &&
		isOnOrForwardPlan(camFrustum.nearFace) &&
		isOnOrForwardPlan(camFrustum.farFace)
		);
}

bool Sphere::isOnOrForwardPlan(const Plane& plan) const
{
	return plan.getSignedDistanceToPlan(center) > -radius;
}

bool Sphere::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	//�O���[�o�����W�n�̃T�C�Y�ɕς���
	const glm::vec3 globalScale = transform.getGlobalScale();

	//�ϊ����邽�߂Ɏg�p���镨�̃Z���^�[�|�W�V�������擾
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center,1.0f) };

	//�`��𐳂����͂��ɂ͍ő�X�P�[���̏�����K�v
	const float maxScale = (std::max)((std::max)(globalScale.x, globalScale.y), globalScale.z);

	//��L�Ŏ擾�������̂͒��a�ɓ�����̂Ŕ����ɂ���
	Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

	//���s�̊m�����������̂��s�����邱�Ƃŏ������y������
	return (
		globalSphere.isOnOrForwardPlan(camFrustum.leftFace) &&
		globalSphere.isOnOrForwardPlan(camFrustum.rightFace) &&
		globalSphere.isOnOrForwardPlan(camFrustum.farFace) &&
		globalSphere.isOnOrForwardPlan(camFrustum.nearFace) &&
		globalSphere.isOnOrForwardPlan(camFrustum.topFace) &&
		globalSphere.isOnOrForwardPlan(camFrustum.bottomFace)
		);
}

bool SquareAABB::isOnOrForwardPlan(const Plane& plan) const
{
	//���e�Ԋu���a���v�Z����
	const float r = extent *
		(std::abs(plan.normal.x) + std::abs(plan.normal.y) + std::abs(plan.normal.z));
	return -r <= plan.getSignedDistanceToPlan(center);
}

bool SquareAABB::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	//�O���[�o�����W�n�̃T�C�Y�ɕς���
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center,1.0f) };

	//�e�傫�����ؓ�����
	const glm::vec3 right = transform.getRight() * extent;
	const glm::vec3 up = transform.getUp() * extent;
	const glm::vec3 forward = transform.getForward() * extent;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, forward));

	const SquareAABB globalAABB(globalCenter, (std::max)((std::max)(newIi, newIj), newIk));

	return globalAABB.isOnOrForwardPlan(camFrustum.leftFace) &&
		globalAABB.isOnOrForwardPlan(camFrustum.rightFace) &&
		globalAABB.isOnOrForwardPlan(camFrustum.topFace) &&
		globalAABB.isOnOrForwardPlan(camFrustum.bottomFace) &&
		globalAABB.isOnOrForwardPlan(camFrustum.nearFace) &&
		globalAABB.isOnOrForwardPlan(camFrustum.farFace);
}

std::vector<glm::vec3> AABB::getVertice()const
{
	std::vector<glm::vec3> vertice{
		{ center.x - extent.x,center.y - extent.y,center.z - extent.z },
		{ center.x + extent.x,center.y - extent.y,center.z - extent.z },
		{ center.x - extent.x,center.y + extent.y,center.z - extent.z },
		{ center.x + extent.x,center.y + extent.y,center.z - extent.z },
		{ center.x - extent.x,center.y - extent.y,center.z + extent.z },
		{ center.x + extent.x,center.y - extent.y,center.z + extent.z },
		{ center.x - extent.x,center.y + extent.y,center.z + extent.z },
		{ center.x + extent.x,center.y + extent.y,center.z + extent.z },
	};

	return vertice;
}

std::array<unsigned int, 36> AABB::getIndices() const
{
	return
	{
		0, 1, 2, 2, 3, 0, // Front face
		1, 5, 6, 6, 2, 1, // Right face
		5, 7, 8, 8, 6, 5, // Back face
		7, 0, 3, 3, 8, 7, // Left face
		3, 2, 6, 6, 8, 3, // Up face
		0, 7, 5, 5, 1, 0, // Down face
	};
}

bool AABB::isOnOrForwardPlan(const Plane& plan) const
{
	const float r = extent.x * std::abs(plan.normal.x) +
		extent.y * std::abs(plan.normal.y) +
		extent.z * std::abs(plan.normal.z);
	return -r <= plan.getSignedDistanceToPlan(center);
}

bool AABB::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center,1.0f) };

	const glm::vec3 right = transform.getRight() * extent.x;
	const glm::vec3 up = transform.getUp() * extent.y;
	const glm::vec3 forward = transform.getForward() * extent.z;

	const float newIi =
		std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 1.0f,0.0f,0.0f }, forward));

	const float newIj =
		std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,1.0f,0.0f }, forward));

	const float newIk =
		std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, right))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, up))
		+ std::abs(glm::dot(glm::vec3{ 0.0f,0.0f,1.0f }, forward));

	const AABB globalAABB(globalCenter, newIi, newIj, newIk);

	return
		(globalAABB.isOnOrForwardPlan(camFrustum.leftFace)) &&
		(globalAABB.isOnOrForwardPlan(camFrustum.rightFace)) &&
		(globalAABB.isOnOrForwardPlan(camFrustum.topFace)) &&
		(globalAABB.isOnOrForwardPlan(camFrustum.bottomFace)) &&
		(globalAABB.isOnOrForwardPlan(camFrustum.nearFace)) &&
		(globalAABB.isOnOrForwardPlan(camFrustum.farFace));
}

bool Intersect(const Sphere& s0, const Sphere& s1, ContactPoint& cp)
{
	// �u���S�Ԃ̋����v���u���a�̍��v�v���傫����΁A�Փ˂��Ă��Ȃ�
	const glm::vec3 d = s0.center - s1.center;
	const float d2 = glm::dot(d, d);
	const float totalRadius = s0.radius + s1.radius;
	if (d2 > totalRadius * totalRadius) {
		return false; // �Փ˂��Ă��Ȃ�
	}

	// �Փ˂��Ă���

	// ���̊Ԃ̋������ق�0�̏ꍇ�A�Փ˕�����������ɐݒ�
	if (d2 < 0.00001f) {
		cp.normal = glm::vec3(0, 1, 0);
		cp.penetration = -totalRadius;
	}
	else {
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - totalRadius;
	}

	// �Փˈʒu�����̂̒��Ԃɐݒ�
	cp.position = s0.center - cp.normal * (s0.radius - cp.penetration * 0.5f);
	return true;
}

bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp)
{
	int n = 0; // �Փ˕���
	cp.penetration = -FLT_MAX; // �ђʋ���

	auto amin = a.center - a.extent;
	auto amax = a.center + a.extent;

	auto bmin = b.center - b.extent;
	auto bmax = b.center + b.extent;

	// �Փ˂������ǂ����𔻒�
	for (int i = 0; i < 3; ++i) {
		const float t0 = amin[i] - bmax[i];
		if (t0 > 0) {
			return false; // �Փ˂��Ă��Ȃ�
		}

		// �ђʋ������Z���������Փ˕����Ƃ���
		if (t0 > cp.penetration) {
			cp.penetration = t0;
			n = i * 2;
		}

		const float t1 = bmin[i] - amax[i];
		if (t1 > 0) {
			return false; // �Փ˂��Ă��Ȃ�
		}
		// �ђʋ������Z���������Փ˕����Ƃ���
		if (t1 > cp.penetration) {
			cp.penetration = t1;
			n = i * 2 + 1;
		}
	}
	// �Փ˕�����ݒ�
	static const glm::vec3 normal[] = {
	  { 1, 0, 0 }, {-1, 0, 0 },
	  { 0, 1, 0 }, { 0,-1, 0 },
	  { 0, 0, 1 }, { 0, 0,-1 }
	};
	cp.normal = normal[n];

	// �Փ˓_���v�Z
	for (int i = 0; i < 3; ++i) {
		cp.position[i] =
			(std::max)(amin[i], bmin[i]) + (std::min)(amax[i], bmax[i]);
	}
	cp.position *= 0.5f;

	return true;
}

Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = zFar * cam.Front;

	frustum.nearFace = { cam.Position + zNear * cam.Front, cam.Front };
	frustum.farFace = { cam.Position + frontMultFar, -cam.Front };
	frustum.rightFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
	frustum.leftFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
	frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
	frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };

	return frustum;
}

AABB generateAABB(const Model& model)
{
	glm::vec3 minAABB = glm::vec3((std::numeric_limits<float>::max)());
	glm::vec3 maxAABB = glm::vec3((std::numeric_limits<float>::min)());
	for (auto&& mesh : model.meshes) {
		for (auto&& vertex : mesh.vertices) {
			minAABB.x = (std::min)(minAABB.x, vertex.position.x);
			minAABB.y = (std::min)(minAABB.y, vertex.position.y);
			minAABB.z = (std::min)(minAABB.z, vertex.position.z);

			maxAABB.x = (std::max)(maxAABB.x, vertex.position.x);
			maxAABB.y = (std::max)(maxAABB.y, vertex.position.y);
			maxAABB.z = (std::max)(maxAABB.z, vertex.position.z);
		}
	}
	return AABB(minAABB, maxAABB);
}

Sphere generateSphereBV(const Model& model)
{
	glm::vec3 minAABB = glm::vec3((std::numeric_limits<float>::max)());
	glm::vec3 maxAABB = glm::vec3((std::numeric_limits<float>::min)());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			minAABB.x = (std::min)(minAABB.x, vertex.position.x);
			minAABB.y = (std::min)(minAABB.y, vertex.position.y);
			minAABB.z = (std::min)(minAABB.z, vertex.position.z);

			maxAABB.x = (std::max)(maxAABB.x, vertex.position.x);
			maxAABB.y = (std::max)(maxAABB.y, vertex.position.y);
			maxAABB.z = (std::max)(maxAABB.z, vertex.position.z);
		}
	}

	return Sphere((maxAABB + minAABB) * 0.5f, (maxAABB.y - minAABB.y)  * 0.5f);
}

Capsule generateCapsule(const Model& model)
{
	glm::vec3 minAABB = glm::vec3((std::numeric_limits<float>::max)());
	glm::vec3 maxAABB = glm::vec3((std::numeric_limits<float>::min)());
	for (auto&& mesh : model.meshes)
	{
		for (auto&& vertex : mesh.vertices)
		{
			minAABB.x = (std::min)(minAABB.x, vertex.position.x);
			minAABB.y = (std::min)(minAABB.y, vertex.position.y);
			minAABB.z = (std::min)(minAABB.z, vertex.position.z);

			maxAABB.x = (std::max)(maxAABB.x, vertex.position.x);
			maxAABB.y = (std::max)(maxAABB.y, vertex.position.y);
			maxAABB.z = (std::max)(maxAABB.z, vertex.position.z);
		}
	}
	float lengthY = glm::length(minAABB.y - maxAABB.y);
	glm::vec3 center = (maxAABB + minAABB) * 0.5f;
	return Capsule(
		glm::vec3(center.x,center.y + (lengthY * 0.5f),center.z),
		glm::vec3(center.x,center.y - (lengthY * 0.5f),center.z),
		(lengthY * 0.5f));
}

glm::vec3 ClosestPoint(const AABB& aabb, const glm::vec3& point)
{
	return glm::vec3(
		std::clamp(point.x, -aabb.extent.x, aabb.extent.x),
		std::clamp(point.y, -aabb.extent.y, aabb.extent.y),
		std::clamp(point.z, -aabb.extent.z, aabb.extent.z)
	);
}

glm::vec3 ClosestPoint(const Box& box, const glm::vec3& point)
{
	glm::vec3 v = point - box.center;
	glm::vec3 p = box.center;
	for (int i = 0; i < 3; ++i) {
		float d = dot(box.axis[i], v);
		d = std::clamp(d, -box.extent[i], box.extent[i]);
		p += box.axis[i] * d;
	}
	return p;
}

glm::vec3 ClosestPoint(const LineSegment& seg, const glm::vec3& point)
{
	// �_������Ɏˉe
	const glm::vec3 d = seg.end - seg.start;
	const float t = glm::dot(point - seg.start, d);

	// �����͈̔͂ɐ���
	if (t <= 0) {
		return seg.start;
	}
	const float d2 = glm::dot(d, d); // �����̒�����2��
	if (t >= d2) {
		return seg.end;
	}

	// �ŋߐړ_���v�Z���ĕԂ�
	return seg.start + d * (t / d2);
}

LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB)
{
	const glm::vec3 d0 = segA.end - segA.start; // ����A�̕����x�N�g��
	const glm::vec3 d1 = segB.end - segB.start; // ����B�̕����x�N�g��
	const glm::vec3 r = segA.start - segB.start;
	const float a = glm::dot(d0, d0); // ����A�̒�����2��
	const float b = glm::dot(d0, d1);
	const float c = glm::dot(d0, r);
	const float e = glm::dot(d1, d1); // ����B�̒�����2��
	const float f = glm::dot(d1, r);

	// �����̒�����0�Ƃ�������P�[�X����������
	// ������0�̐������u�_�v�Ƃ݂Ȃ��Čv�Z

	// ����A��B�̗����̒�����0
	if (a < 0.0001f && e < 0.0001f) {
		return { segA.start, segB.start };
	}

	// ����A�̒�����0
	if (a < 0.0001f) {
		if (f <= 0) {
			return { segA.start, segB.start };
		}
		else if (f >= e) {
			return { segA.start, segB.end };
		}
		return { segA.start, segB.start + d1 * (f / e) };
	}

	// ����B�̒�����0
	if (e < 0.0001f) {
		if (-c <= 0) {
			return { segA.start, segB.start };
		}
		else if (-c >= a) {
			return { segA.end, segB.start };
		}
		return { segA.start + d0 * (-c / a), segB.start };
	}

	// �����̒�����0���傫���ƕ���������A���������s���ǂ������`�F�b�N����
	// ���������s�ȏꍇ�As�����߂鎮�̕��ꂪ0�ɂȂ�v�Z�ł��Ȃ�����
	const float denom = a * e - b * b;
	if (denom) {
		// ����A�̍ŋߐړ_�����߁A����A�͈̔͂ɐ���
		float s = std::clamp((f * b - c * e) / denom, 0.0f, 1.0f);

		// ����B�̍ŋߐړ_�����߂�
		float t = b * s + f;

		// t������B�̊O�����w���Ă���ꍇ
		// t�����B�̒[�_�ɐݒ肵�As���v�Z���Ȃ���
		LineSegment result;
		if (t < 0) {
			result.end = segB.start;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else if (t > e) {
			result.end = segB.end;
			s = std::clamp((b - c) / a, 0.0f, 1.0f);
		}
		else {
			result.end = segB.start + d1 * (t / e);
		}
		result.start = segA.start + d0 * s;
		return result;
	}

	// ���������s�ȏꍇ�A�����̏d������͈͂̒��_��I��

	// ����B�����A�Ɏˉe
	float u = glm::dot(d0, segB.start - segA.start);
	float v = glm::dot(d0, segB.end - segA.start);

	// �ˉe�������W�����A�͈̔͂ɐ���
	u = std::clamp(u, 0.0f, a);
	v = std::clamp(v, 0.0f, a);

	// u��v�̒��_������A�̍ŋߐړ_�ɂȂ�
	float s = (u + v) / a * 0.5f;
	LineSegment result;
	result.start = segA.start + d0 * s;

	// ����A�̍ŋߐړ_�����B�Ɏˉe
	const float t = dot(d1, result.start - segB.start);

	// �ˉe�������W�����B�͈̔͂ɐ���
	if (t <= 0) {
		result.end = segB.start;
	}
	else if (t >= e) {
		result.end = segB.end;
	}
	else {
		result.end = segB.start + d1 * (t / e);
	}

	return result;
}

LineSegment ClosestPoint(const struct Rectangle& rect, const LineSegment& seg)
{
	// �����̎n�_��ʂɎˉe
	const glm::vec3 ca = seg.start - rect.center;
	const float a0 = glm::dot(ca, rect.axis[0]);
	const float a1 = glm::dot(ca, rect.axis[1]);

	// �����`�Ɛ����̕��s���𒲂ׂ�
	const glm::vec3 ab = seg.end - seg.start;
	const float n = glm::dot(rect.normal, ab);
	if (abs(n) <= 0.0001f) {
		// ���s�ȏꍇ�A�����������`�̏�ɏ���Ă��邩�ǂ����𒲂ׂ�
		float tmin = -FLT_MAX;
		float tmax = FLT_MAX;
		const float list[] = { a0, a1 };
		for (int i = 0; i < 2; ++i) {
			// ������axis[i]�Ƃ̕��s���𒲂ׂ�
			const float d = glm::dot(ab, rect.axis[i]);
			if (abs(d) <= 0.0001f) {
				continue; // ���s�Ȃ̂Ō�_���Ȃ�
			}
			// �����`���S����̋���������̔䗦�Ƃ��ĕ\��
			float t0 = (-rect.scale[i] - list[i]) / d;
			float t1 = (rect.scale[i] - list[i]) / d;
			if (t0 > t1) {
				std::swap(t0, t1);
			}
			// �����`�ƌ������Ă���͈͂��v�Z
			tmin = (std::max)(t0, tmin);
			tmax = (std::min)(t1, tmax);
		}

		// �����̎n�_�ƏI�_�͈̔͂ɐ�������
		tmin = (std::max)(0.0f, tmin);
		tmax = (std::min)(1.0f, tmax);

		// �����������`�ɏd�Ȃ��Ă���ꍇ
		// �d���͈͂̒��S���ŋߐړ_�Ƃ���
		if (tmin <= tmax) {
			const glm::vec3 p = seg.start + ab * ((tmin + tmax) * 0.5f);
			const float d = dot(rect.normal, p - rect.center);
			return { p, p - rect.normal * d };
		}
	} // if (abs(n) <= 0.0001f)

	// �������ʂƌ������Ă��邩�`�F�b�N
	// �������Ă���ꍇ�A�ŋߐړ_�͌�_�ɂȂ�
	{
		const float d = glm::dot(rect.normal, rect.center);
		const float t = (d - glm::dot(rect.normal, seg.start)) / n;
		if (0 <= t && t <= 1) {
			// ��_�������`�͈͓̔��ɂ��邩���ׂ�
			const glm::vec3 p = seg.start + ab * t;
			const glm::vec3 v = p - rect.center;
			const float dist1 = glm::dot(v, rect.axis[0]);
			if (abs(dist1) <= rect.scale[0]) {
				const float dist2 = glm::dot(v, rect.axis[1]);
				if (abs(dist2) <= rect.scale[1]) {
					return { p, p };
				}
			}
		} // if (0 <= t && t <= n)
	}

	// ���s�łȂ��A���������Ă��Ȃ��ꍇ

	// �����̏I�_��ʂɎˉe
	const glm::vec3 cb = seg.end - rect.center;
	const float b0 = glm::dot(cb, rect.axis[0]);
	const float b1 = glm::dot(cb, rect.axis[1]);

	// �����̎n�_����яI�_�̍ŋߐړ_������
	// �����`�͈̔͂Ɋ܂܂��_�̂������߂��ق����u���̍ŋߐړ_�v�Ƃ���
	float distance = FLT_MAX;
	LineSegment result;
	if (abs(a0) <= rect.scale[0] && abs(a1) <= rect.scale[1]) {
		distance = dot(rect.normal, ca);
		distance *= distance; // ��r�̂��߂�2�悷��
		result = { seg.start, rect.center + rect.axis[0] * a0 + rect.axis[1] * a1 };
	}
	if (abs(b0) <= rect.scale[0] && abs(b1) <= rect.scale[1]) {
		float tmp = dot(rect.normal, cb);
		tmp *= tmp; // ��r�̂��߂�2�悷��
		if (tmp < distance) {
			distance = tmp;
			result = { seg.end, rect.center + rect.axis[0] * b0 + rect.axis[1] * b1 };
		}
	}

	// �����`��4�ӂɂ��āA�ŋߐړ_���v�Z
	const glm::vec3 v1 = rect.axis[0] * rect.scale[0];
	const glm::vec3 v2 = rect.axis[1] * rect.scale[1];
	const glm::vec3 v[] = {
	  rect.center + v1 + v2,
	  rect.center + v1 - v2,
	  rect.center - v1 - v2,
	  rect.center - v1 + v2 };
	for (int i = 0; i < 4; ++i) {
		const auto s = ClosestPoint(seg, LineSegment{ v[i], v[(i + 1) % 4] });
		const glm::vec3 ab = s.end - s.start;
		const float distanceEdge = glm::dot(ab, ab); // ������2��
		if (distanceEdge < distance) { // 2�擯�m�̔�r
			distance = distanceEdge;
			result = s;
		}
	}
	return result;
}

void ProjectBoxToAxis(const glm::vec3* vertices, const glm::vec3& axis, float& min, float& max)
{
	min = max = glm::dot(axis, vertices[0]);
	for (int i = 0; i < 8; ++i) {
		float f = glm::dot(axis, vertices[i]);
		min = std::min(min, f);
		max = std::max(max, f);
	}
}

float GetIntersectionLength(const glm::vec3* verticesA, const glm::vec3* verticesB, const glm::vec3& axis, bool& shouldFlip)
{
	// 2��OBB�����Ɏˉe
	float minA, maxA, minB, maxB;
	ProjectBoxToAxis(verticesA, axis, minA, maxA);
	ProjectBoxToAxis(verticesB, axis, minB, maxB);

	// �ˉe�����͈͂��������Ă��Ȃ��Ȃ�0��Ԃ�
	if (minB > maxA && minA > maxB) {
		return 0;
	}

	// �Փ˖@���́uOBB����1�v�����A�uOBB����2�v���E�ɂ���Ƒz�肵�Ă���̂ŁA
	// �������t�ɂȂ��Ă���ꍇ�A�Փ˖@�{�̌����𔽓]����K�v������A
	// �Ƃ������Ƃ��A�Ăяo�����ɓ`����
	shouldFlip = minB < minA;

	// �������Ă��鋗�����v�Z
	float lenA = maxA - minA;
	float lenB = maxB - minB;
	float min = std::min(minA, minB);
	float max = std::max(maxA, maxB);
	float length = max - min;
	return length - lenA - lenB;
}

std::vector<LineSegment> GetEdges(const glm::vec3* vertices)
{
	return {
		// X������
		{ vertices[0], vertices[1] },
		{ vertices[2], vertices[3] },
		{ vertices[4], vertices[5] },
		{ vertices[6], vertices[7] },

		// Y������
		{ vertices[0], vertices[2] },
		{ vertices[1], vertices[3] },
		{ vertices[4], vertices[6] },
		{ vertices[5], vertices[7] },

		// Z������
		{ vertices[0], vertices[4] },
		{ vertices[1], vertices[5] },
		{ vertices[2], vertices[6] },
		{ vertices[3], vertices[7] },
	};
}

std::vector<Plane> GetPlanes(const Box& box)
{
	const glm::vec3 ax = box.axis[0] * box.extent.x;
	const glm::vec3 ay = box.axis[1] * box.extent.y;
	const glm::vec3 az = box.axis[2] * box.extent.z;
	return std::vector<Plane>{
		{ box.axis[0], glm::dot(box.axis[0], box.center + ax) }, // +x
		{ -box.axis[0],-glm::dot(box.axis[0], box.center - ax) }, // -x
		{ box.axis[1], glm::dot(box.axis[1], box.center + ay) }, // +y
		{ -box.axis[1],-glm::dot(box.axis[1], box.center - ay) }, // -y
		{ box.axis[2], glm::dot(box.axis[2], box.center + az) }, // +z
		{ -box.axis[2],-glm::dot(box.axis[2], box.center - az) }, // -z
	};
}

bool IsPointInsideBox(const glm::vec3& point, const Box& box)
{
	const glm::vec3 v = point - box.center;
	for (int i = 0; i < 3; ++i) {
		const float d = glm::dot(v, box.axis[i]);
		if (d < -box.extent[i] || d > box.extent[i]) {
			return false; // �_��OBB�̊O���ɗL��
		}
	}
	return true; // �_��OBB�̓����ɂ���
}

bool ClipLineSegment(const LineSegment& seg, const Plane& plane, glm::vec3& point)
{
	// �Ӄx�N�g����ʖ@���Ɏˉe
	const glm::vec3 ab = seg.end - seg.start;
	float n = glm::dot(plane.normal, ab);
	if (abs(n) < 0.0001f) {
		return false; // ���s�Ȃ̂Ō����Ȃ�
	}

	// ��_�܂ł̋��������߂�
	float t = plane.distance - dot(plane.normal, seg.start);
	if (t < 0 || t > n) {
		return false; // ��_�������̊O�ɂ���̂Ō����Ȃ�
	}

	// ��_���W���v�Z
	t /= n; // �x�N�g��ab�ɑ΂���䗦�ɕϊ�
	point = seg.start + ab * t;
	return true;
}

std::vector<glm::vec3> ClipEdges(const std::vector<glm::vec3>& vertices, const Box& box)
{
	// �ӂƖʂ��擾
	const auto edges = GetEdges(vertices.data());
	const auto planes = GetPlanes(box);

	// ��_�p�̃�������\��
	// 2��OBB�������[���������Ă��Ȃ�����A��_�̐��͕ӂ̐��ȉ��ɂȂ�͂�
	std::vector<glm::vec3> result;
	result.reserve(edges.size());

	// �ӂƖʂ̌�_�����߂�
	for (const auto& plane : planes) {
		for (const auto& edge : edges) {
			glm::vec3 p;
			if (ClipLineSegment(edge, plane, p)) {
				// ��_��OBB����2�̓����ɂ���Ȃ�A�L���Ȍ�_�Ƃ��ċL�^
				if (IsPointInsideBox(p, box)) {
					result.push_back(p);
				}
			}
		} // for edges
	} // for planes
	return result;
}

float GetIntersectionLength(const Capsule& capsule, const glm::vec3* verticesB, const glm::vec3& axis, bool& shouldFlip, int& minIndex, int& maxIndex)
{
	// �J�v�Z�������Ɏˉe
	float minA, maxA;
	minA = maxA = glm::dot(axis, capsule.seg.start);
	float f = glm::dot(axis, capsule.seg.end);
	minA = (std::min)(minA, f) - capsule.radius;
	maxA = (std::max)(maxA, f) + capsule.radius;

	// OBB�����Ɏˉe
	minIndex = maxIndex = 0;
	float minB, maxB;
	minB = maxB = dot(axis, verticesB[0]);
	for (int i = 1; i < 8; ++i) {
		float f = dot(axis, verticesB[i]);
		if (f < minB) {
			minB = f;
			minIndex = i; // �ŏ��l�̃C���f�b�N�X���L�^
		}
		if (f > maxB) {
			maxB = f;
			maxIndex = i; // �ő�l�̃C���f�b�N�X���L�^
		}
	} // for

	 // ������0�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�܂�������Ă��Ȃ�)
	if (minB > maxA && minA > maxB) {
		return false;
	}

	// �Փ˖@���́uOBB����1�v�����A�uOBB����2�v���E�ɂ���Ƒz�肵�Ă���̂ŁA
	// �������t�ɂȂ��Ă���ꍇ�A�Փ˖@�{�̌����𔽓]����K�v������A
	// �Ƃ������Ƃ��A�Ăяo�����ɓ`����
	shouldFlip = minB < minA;

	// �������Ă��鋗�����v�Z
	float lenA = maxA - minA;
	float lenB = maxB - minB;
	float min = std::min(minA, minB);
	float max = std::max(maxA, maxB);
	float length = max - min;
	return length - lenA - lenB;
}

bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp)
{
	// �ŋߐړ_���狅�̂̒��S�܂ł̋������A���̂̔��a���傫����ΏՓ˂��Ă��Ȃ�
	const glm::vec3 p = ClosestPoint(aabb, sphere.center); // �ŋߐړ_
	const glm::vec3 d = sphere.center - p; // �ŋߐړ_���狅�̂̒��S�ւ̃x�N�g��
	const float d2 = dot(d, d); // ������2��
	if (d2 > sphere.radius * sphere.radius) {
		return false; // �Փ˂��Ă��Ȃ�
	}
	if (d2 < 0.00001f) {
		// ������0�̏ꍇ�A���̂̒��S��AABB�̓����ɂ���
		// ���̏ꍇ�A�ђʋ������ł��Z���ʂ���Փ˂����Ƃ݂Ȃ�
		int n = 0;
		cp.penetration = -FLT_MAX;
		for (int i = 0; i < 3; ++i) {
			float t0 = p[i] - (aabb.extent[i]);
			if (t0 > cp.penetration) {
				n = i * 2;
				cp.penetration = t0;
			}
			float t1 = (-aabb.extent[i]) - p[i];
			if (t1 > cp.penetration) {
				n = i * 2 + 1;
				cp.penetration = t1;
			}
		}

		static const glm::vec3 normal[] = {
		  { 1, 0, 0 }, {-1, 0, 0 },
		  { 0, 1, 0 }, { 0,-1, 0 },
		  { 0, 0, 1 }, { 0, 0,-1 }
		};
		cp.normal = normal[n];
	}
	else {
		// ������0���傫���ꍇ�A���̂̒��S��AABB�̊O���ɂ���
		// ���̏ꍇ�A�ŋߐڂ��狅�̂̒��S�֌�������������Փ˂����Ƃ݂Ȃ�
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - sphere.radius;
	}
	cp.position = p - cp.normal * cp.penetration * 0.5f;
	return true;
}

bool Intersect(const AABB& aabb, const Sphere& sphere, ContactPoint& cp)
{
	if (Intersect(sphere, aabb, cp)) {
		cp.normal = -cp.normal;
		return true;
	}
	return false;
}

bool Intersect(const Sphere& sphere, const Box& box, ContactPoint& cp)
{
	// �ŋߐړ_���狅�̂̒��S�܂ł̋������A���̂̔��a���傫����ΏՓ˂��Ă��Ȃ�
	glm::vec3 p = ClosestPoint(box, sphere.center);
	glm::vec3 d = sphere.center - p;
	float d2 = dot(d, d);
	if (d2 > sphere.radius * sphere.radius) {
		return false;
	}

	if (d2 < 0.00001f) {
		// ������0�̏ꍇ�A���̂̒��S��OBB�̓����ɂ���
		// ���̏ꍇ�A�ђʋ������ł��Z���ʂ���Փ˂����Ƃ݂Ȃ�
		const glm::vec3 a = p - box.center; // OBB���S���狅�̒��S�ւ̃x�N�g��
		cp.penetration = -FLT_MAX;
		for (int i = 0; i < 3; ++i) {
			float f = dot(box.axis[i], a); // �ŗL���֎ˉe
			float t0 = f - box.extent[i];
			if (t0 > cp.penetration) {
				cp.normal = box.axis[i];
				cp.penetration = t0;
			}
			float t1 = -box.extent[i] - f;
			if (t1 > cp.penetration) {
				cp.normal = -box.axis[i];
				cp.penetration = t1;
			}
		}
	}
	else {
		// ������0���傫���ꍇ�A���̂̒��S��OBB�̊O���ɂ���
		// ���̏ꍇ�A�ŋߐڂ��狅�̂̒��S�֌�������������Փ˂����Ƃ݂Ȃ�
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - sphere.radius;
	}
	cp.position = p - cp.normal * cp.penetration * 0.5f;

	return true;
}

bool Intersect(const Box& box, const Sphere& sphere, ContactPoint& cp)
{
	if (Intersect(sphere, box, cp)) {
		cp.normal = -cp.normal;
		return true;

	}
	return false;
}

bool Intersect(const Box& boxA, const Box& boxB, ContactPoint& cp)
{
	// OBB�̒��_���W�z����擾
	const auto verticesA = boxA.getVertice();
	const auto verticesB = boxB.getVertice();

	// �ђʋ������ő�ɂ��Ă���
	cp.penetration = -FLT_MAX;

	// ����������p�̎��x�N�g�����v�Z
	glm::vec3 axisList[15] = {
	  boxA.axis[0], boxA.axis[1], boxA.axis[2],
	  -boxB.axis[0], -boxB.axis[1], -boxB.axis[2],
	};
	for (int i = 0; i < 3; ++i) {
		axisList[6 + i * 3 + 0] = cross(axisList[i], axisList[3]);
		axisList[6 + i * 3 + 1] = cross(axisList[i], axisList[4]);
		axisList[6 + i * 3 + 2] = cross(axisList[i], axisList[5]);
	}

	// ���ׂĂ̎��x�N�g���ɑ΂��ĕ�������������s
	for (int i = 0; i < 15; ++i) {
		// �O�ςɂ���č��ꂽ���x�N�g���̏ꍇ�A���̎������s���ƒ�����0�ɂȂ�
		// ���̏ꍇ�AA�܂���B�̎��ɂ�镪��������Ɠ������̂ŃX�L�b�v����
		const float length2 = dot(axisList[i], axisList[i]);
		if (length2 < 0.0001f) {
			continue;
		}

		// �������Ă��鋗�����v�Z
		bool shouldFlip;
		float d = GetIntersectionLength(
			verticesA.data(), verticesB.data(), axisList[i], shouldFlip);

		// ������0�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�܂�������Ă��Ȃ�)
		if (d >= 0) {
			return false; // �Փ˂��Ă��Ȃ�
		}

		// �O�ςō쐬�������x�N�g���̒�����1�ɂȂ�Ȃ�(sin�ƂȂ̂�)
		// ���ʂƂ��āAd�͎��x�N�g���̒�����P�ʂƂ����l�ɂȂ�
		// �������A�P�ʂ��قȂ�ƒ������r�ł��Ȃ����߁A���K�����ĒP�ʂ𑵂���
		d /= std::sqrt(length2);

		// �������������Z����������Փ˂����Ƃ݂Ȃ�
		if (d > cp.penetration) {
			cp.penetration = d;
			// �Փ˖@���͎��x�N�g���̋t����
			if (shouldFlip) {
				cp.normal = axisList[i];
			}
			else {
				cp.normal = -axisList[i];
			}
		}
	}

	// �Փ˖@���𐳋K��
	cp.normal = normalize(cp.normal);

	// �R���^�N�g�|�C���g���v�Z

 // OBB��OBB�̌�_�����߂�
	std::vector<glm::vec3> c0 = ClipEdges(verticesB, boxA);
	std::vector<glm::vec3> c1 = ClipEdges(verticesA, boxB);
	c0.insert(c0.end(), c1.begin(), c1.end());

	// �ђʋ����̒��ԂɏՓ˕��ʂ�ݒ�(�Փ˕��ʏ�̓_p�����߂�)
	float min, max;
	ProjectBoxToAxis(verticesA.data(), cp.normal, min, max);
	const glm::vec3 p = boxA.center - cp.normal * (max - min + cp.penetration) * 0.5f;

	// ��_���Փ˕��ʂɎˉe���A�d�������_���폜
	int count = 0;
	for (int i = 0; i < c0.size(); ++i, ++count) {
		// ��_���Փ˕��ʂɎˉe
		c0[count] = c0[i] + cp.normal * dot(cp.normal, p - c0[i]);

		// �d�������_������ꍇ�͍폜
		for (int j = 0; j < count; ++j) {
			const glm::vec3 v = c0[j] - c0[count];
			if (glm::dot(v, v) < 0.0001f) {
				--count;
				break;
			}
		}
	}
	c0.resize(count);

	// ��_�̏d�S���v�Z���A�R���^�N�g�|�C���g�Ƃ���
	cp.position = glm::vec3(0);
	for (const auto& e : c0) {
		cp.position += e;
	}
	cp.position /= static_cast<float>(c0.size());

	return true; // �Փ˂��Ă���
}

bool Intersect(const Box& box, const AABB& aabb, ContactPoint& cp)
{
	Box boxB;
	boxB.center = aabb.center;
	boxB.extent = aabb.extent;
	return Intersect(box, boxB, cp);
}

bool Intersect(const AABB& aabb, const Box& box, ContactPoint& cp)
{
	Box boxA;
	boxA.center = aabb.center;
	boxA.extent = aabb.extent;
	return Intersect(boxA, box, cp);
}

bool Intersect(const Sphere& sphere, const Capsule& capsule, ContactPoint& cp)
{
	const glm::vec3 p = ClosestPoint(capsule.seg, sphere.center);
	return Intersect(sphere, Sphere{ p, capsule.radius }, cp);
}

bool Intersect(const Capsule& capsule, const Sphere& sphere, ContactPoint& cp)
{
	const glm::vec3 p = ClosestPoint(capsule.seg, sphere.center);
	return Intersect(Sphere{ p, capsule.radius }, sphere, cp);
}

bool Intersect(const Capsule& capA, const Capsule& capB, ContactPoint& cp)
{
	const LineSegment seg = ClosestPoint(capA.seg, capB.seg);
	const glm::vec3 d = seg.start - seg.end;
	const float d2 = glm::dot(d, d);
	const float totalRadius = capA.radius + capB.radius;
	if (d2 > totalRadius * totalRadius) {
		return false;
	}

	// �ŋߐړ_���m�̋�����0���傫���ꍇ
	// ���̓��m�Ɠ������@�ŃR���^�N�g�|�C���g���v�Z����
	if (d2 >= 0.00001f) {
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - totalRadius;
		cp.position = seg.start - cp.normal * (capA.radius - cp.penetration * 0.5f);
		return true;
	}

	// �ŋߐړ_���m�̋�����0�̏ꍇ
	// seg����@�����v�Z�ł��Ȃ��̂ŁA2���ɐ����ȕ������Փ˖@���Ƃ���
	const glm::vec3 d0 = capA.seg.end - capA.seg.start;
	const glm::vec3 d1 = capB.seg.end - capB.seg.start;
	cp.normal = glm::cross(d0, d1);
	if (dot(cp.normal, cp.normal) < 0.00001f) {
		// �O�ςɂ���ē����@���x�N�g���̒�����0�̏ꍇ�A2���͕��s
		// �@���x�N�g�������Ȃ��̂ŁAd0�x�N�g���ɐ����ȕ������Փ˖@���Ƃ���
		if (d0.x) {
			cp.normal = { -(d0.y + d0.z) / d0.x, 1, 1 };
		}
		else if (d0.y) {
			cp.normal = { 1, -(d0.x + d0.z) / d0.y, 1 };
		}
		else {
			cp.normal = { 1, 1, -(d0.x + d0.y) / d0.z };
		}
	}
	cp.normal = normalize(cp.normal);

	// �傫�����̔��a���ђʋ����Ƃ���
	cp.penetration = -std::max(capA.radius, capB.radius);

	// �R���^�N�g�|�C���g�̍��W���v�Z
	cp.position = seg.start - cp.normal * (capA.radius - cp.penetration * 0.5f);

	return true;
}

bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp)
{
	// OBB�̒��_���W�z����擾
	const std::vector<glm::vec3> verticesA = { capsule.seg.start, capsule.seg.end };
	const auto verticesB = box.getVertice();

	// ����������p�̎��x�N�g�����v�Z
	const glm::vec3 capsuleDirection = capsule.seg.end - capsule.seg.start;
	const glm::vec3 axis0 = box.axis[0] * box.extent[0];
	const glm::vec3 axis1 = box.axis[1] * box.extent[1];
	const glm::vec3 axis2 = box.axis[2] * box.extent[2];
	const glm::vec3 axisList[] = {
	  box.axis[0], box.axis[1], box.axis[2],           // 0, 1, 2
	  cross(box.axis[0], capsuleDirection),            // 3
	  cross(box.axis[1], capsuleDirection),            // 4
	  cross(box.axis[2], capsuleDirection),            // 5
	  cross(axis0 + axis1 + axis2, capsuleDirection),  // 6
	  cross(axis0 + axis1 - axis2, capsuleDirection),  // 7
	  cross(axis0 - axis1 + axis2, capsuleDirection),  // 8
	  cross(axis0 - axis1 - axis2, capsuleDirection),  // 9
	};

	// �ђʋ������ő�ɂ��Ă���
	cp.penetration = -FLT_MAX;

	// ���ׂĂ̎��x�N�g���ɑ΂��ĕ�������������s
	int axisIndex = 0; // �Փ˂����������̃C���f�b�N�X
	int minIndex = 0, maxIndex = 0;
	for (int i = 0; i < std::size(axisList); ++i) {
		// �O�ςɂ���č��ꂽ���x�N�g���̏ꍇ�A���̎������s���ƒ�����0�ɂȂ�
		// ���̏ꍇ�AA�܂���B�̎��ɂ�镪��������Ɠ������̂ŃX�L�b�v����
		const float length2 = dot(axisList[i], axisList[i]);
		if (length2 < 0.0001f) {
			continue;
		}

		// �������𐳋K��
		const glm::vec3 axis = axisList[i] * (1.0f / sqrt(length2));

		// �������Ă��鋗�����v�Z
		bool shouldFlip;
		int tmpMinIndex, tmpMaxIndex;
		const float d = GetIntersectionLength(capsule, verticesB.data(),
			axis, shouldFlip, tmpMinIndex, tmpMaxIndex);

		// ������0�ȏ�Ȃ番�������ʂ����錄�Ԃ�����(�܂�������Ă��Ȃ�)
		if (d >= 0) {
			return false;
		}

		// �ђʋ��������Z����������Փ˂����Ƃ݂Ȃ�
		if (d > cp.penetration) {
			cp.penetration = d;
			// �Փ˖@���͎��x�N�g���̋t����
			if (shouldFlip) {
				cp.normal = axis;
			}
			else {
				cp.normal = -axis;
			}
			minIndex = tmpMinIndex;
			maxIndex = tmpMaxIndex;
			axisIndex = i;
		}
	} // for

	 // ���������̌������m�F
	const glm::vec3 list[] = { capsule.seg.start, capsule.seg.end };
	float direction = 1; // �J�v�Z�����̌��������p
	for (const auto& e : list) {
		// OBB�̍ŋߐړ_���������ɂȂ��ꍇ�͖���
		const glm::vec3 c = ClosestPoint(box, e);
		const glm::vec3 v = c - e;
		const float p = glm::dot(capsuleDirection, v) * direction;
		direction *= -1;
		if (p >= 0) {
			continue;
		}
		// �ŋߐړ_�܂ł̋������J�v�Z���̔��a���傫���ꍇ�͖���
		float t = glm::dot(v, v); // ������2��
		if (t > capsule.radius * capsule.radius) {
			continue;
		}
		// �ŋߐړ_�܂ł̋������������ɂ�鋗�����Z���ꍇ�̂ݔ����Ƃ̏Փ˂�I��
		t = sqrt(t); // 2��̏�Ԃ�����
		const float d = t - capsule.radius;
		if (d > cp.penetration) {
			cp.penetration = d;
			cp.normal = -v * (1.0f / t);
			cp.position = c + cp.normal * (cp.penetration * 0.5f);
			axisIndex = -1;
		}
	} // for

	// �R���^�N�g�|�C���g���v�Z
	// �������ɂ���ďꍇ����
	switch (axisIndex) {
	case 0:
	case 1:
	case 2: {
		// �ʂŏՓ�
		// �����̎n�_����яI�_�ɂ��āA�ʏ�̍ŋߐړ_���v�Z
		// �ŋߐړ_��OBB�͈͓̔��ɂ���ꍇ�̂ݗL��
		const int axisIndex1 = (axisIndex + 1) % 3;
		const int axisIndex2 = (axisIndex + 2) % 3;
		const Rectangle rect = {
		  box.center + cp.normal * box.extent[axisIndex],
		  cp.normal,
		  box.axis[axisIndex1], box.axis[axisIndex2],
		  box.extent[axisIndex1], box.extent[axisIndex2] };
		const auto seg = ClosestPoint(rect, capsule.seg);
		const glm::vec3 v = seg.start - seg.end;
		// �����Ƃ��Ĉ����钷��������΁A��������Փ˖@�������߂�
		// �قړ_�Ȃ番�������ʂ̖@���𗘗p����
		if (glm::dot(v, v) >= 0.0001f) {
			cp.normal = normalize(v);
		}
		cp.position = seg.end + cp.normal * (cp.penetration * 0.5f);
		break;
	}
	case 3:
	case 4:
	case 5: {
		// �ӂŏՓ�
		static const int pairList[][8] = {
			// 0, 1, 2, 3, 4, 5, 6, 7
			 { 1, 0, 3, 2, 5, 4, 7, 6 },
			 { 2, 3, 0, 1, 6, 7, 4, 5 },
			 { 4, 5, 6, 7, 0, 1, 2, 3 },
		};
		int index0 = 0;
		const float a = glm::dot(cp.normal, capsule.seg.start - verticesB[minIndex]);
		const float b = glm::dot(cp.normal, capsule.seg.start - verticesB[maxIndex]);
		if (a < b) {
			index0 = minIndex;
		}
		else {
			index0 = maxIndex;
		}
		const int index1 = pairList[axisIndex - 3][index0];
		const auto seg = ClosestPoint(capsule.seg,
			LineSegment{ verticesB[index0], verticesB[index1] });
		cp.position = seg.end + cp.normal * (cp.penetration * 0.5f);
		break;
	}
	case 6:
	case 7:
	case 8:
	case 9: {
		// ���_�ŏՓ�
		const float a = glm::dot(cp.normal, capsule.seg.start - verticesB[minIndex]);
		const float b = glm::dot(cp.normal, capsule.seg.start - verticesB[maxIndex]);
		if (a < b) {
			cp.position = verticesB[minIndex] + cp.normal * (cp.penetration * 0.5f);
		}
		else {
			cp.position = verticesB[maxIndex] + cp.normal * (cp.penetration * 0.5f);
		}
		break;
		}
	} // switch
	return true;
}

bool Intersect(const Box& box, const Capsule& capsule, ContactPoint& cp)
{
	if (Intersect(capsule, box, cp)) {
		cp.normal = -cp.normal;
		return true;
	}
	return false;
}

bool Intersect(const Capsule& capsule, const AABB& aabb, ContactPoint& cp)
{
	Box boxB;
	boxB.center = aabb.center;
	boxB.extent = aabb.extent;	
	return Intersect(capsule, boxB, cp);
}

bool Intersect(const AABB& aabb, const Capsule& capsule, ContactPoint& cp)
{
	Box boxA;
	boxA.center = aabb.center;
	boxA.extent = aabb.extent;
	return Intersect(boxA,capsule,cp);
}

std::vector<glm::vec3> Box::getVertice() const
{
	const glm::vec3 ax = axis[0] * extent.x;
	const glm::vec3 ay = axis[1] * extent.y;
	const glm::vec3 az = axis[2] * extent.z;
	return {
		center + ax + ay + az, // 0
		center - ax + ay + az, // 1
		center + ax - ay + az, // 2
		center - ax - ay + az, // 3
		center + ax + ay - az, // 4
		center - ax + ay - az, // 5
		center + ax - ay - az, // 6
		center - ax - ay - az  // 7
	};
}

std::array<unsigned int, 36> Box::getIndices() const
{
	return std::array<unsigned int, 36>();
}

bool Box::isOnOrForwardPlan(const Plane& plan) const
{
	return false;
}

bool Box::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	return false;
}

bool Capsule::isOnOrForwardPlan(const Plane& plan) const
{
	return false;
}

bool Capsule::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	return false;
}