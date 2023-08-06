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
	//グローバル座標系のサイズに変える
	const glm::vec3 globalScale = transform.getGlobalScale();

	//変換するために使用する物のセンターポジションを取得
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center,1.0f) };

	//形状を正しく囲うには最大スケールの上方が必要
	const float maxScale = (std::max)((std::max)(globalScale.x, globalScale.y), globalScale.z);

	//上記で取得したものは直径に当たるので半分にしる
	Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

	//失敗の確率が高いものを先行させることで処理を軽くする
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
	//投影間隔半径を計算する
	const float r = extent *
		(std::abs(plan.normal.x) + std::abs(plan.normal.y) + std::abs(plan.normal.z));
	return -r <= plan.getSignedDistanceToPlan(center);
}

bool SquareAABB::isOnFrustum(const Frustum& camFrustum, const Transform& transform) const
{
	//グローバル座標系のサイズに変える
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center,1.0f) };

	//各大きさを借入する
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
	// 「中心間の距離」が「半径の合計」より大きければ、衝突していない
	const glm::vec3 d = s0.center - s1.center;
	const float d2 = glm::dot(d, d);
	const float totalRadius = s0.radius + s1.radius;
	if (d2 > totalRadius * totalRadius) {
		return false; // 衝突していない
	}

	// 衝突している

	// 球体間の距離がほぼ0の場合、衝突方向を上向きに設定
	if (d2 < 0.00001f) {
		cp.normal = glm::vec3(0, 1, 0);
		cp.penetration = -totalRadius;
	}
	else {
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - totalRadius;
	}

	// 衝突位置を球体の中間に設定
	cp.position = s0.center - cp.normal * (s0.radius - cp.penetration * 0.5f);
	return true;
}

bool Intersect(const AABB& a, const AABB& b, ContactPoint& cp)
{
	int n = 0; // 衝突方向
	cp.penetration = -FLT_MAX; // 貫通距離

	auto amin = a.center - a.extent;
	auto amax = a.center + a.extent;

	auto bmin = b.center - b.extent;
	auto bmax = b.center + b.extent;

	// 衝突したかどうかを判定
	for (int i = 0; i < 3; ++i) {
		const float t0 = amin[i] - bmax[i];
		if (t0 > 0) {
			return false; // 衝突していない
		}

		// 貫通距離が短い方向を衝突方向とする
		if (t0 > cp.penetration) {
			cp.penetration = t0;
			n = i * 2;
		}

		const float t1 = bmin[i] - amax[i];
		if (t1 > 0) {
			return false; // 衝突していない
		}
		// 貫通距離が短い方向を衝突方向とする
		if (t1 > cp.penetration) {
			cp.penetration = t1;
			n = i * 2 + 1;
		}
	}
	// 衝突方向を設定
	static const glm::vec3 normal[] = {
	  { 1, 0, 0 }, {-1, 0, 0 },
	  { 0, 1, 0 }, { 0,-1, 0 },
	  { 0, 0, 1 }, { 0, 0,-1 }
	};
	cp.normal = normal[n];

	// 衝突点を計算
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
	// 点を線分に射影
	const glm::vec3 d = seg.end - seg.start;
	const float t = glm::dot(point - seg.start, d);

	// 線分の範囲に制限
	if (t <= 0) {
		return seg.start;
	}
	const float d2 = glm::dot(d, d); // 線分の長さの2乗
	if (t >= d2) {
		return seg.end;
	}

	// 最近接点を計算して返す
	return seg.start + d * (t / d2);
}

LineSegment ClosestPoint(const LineSegment& segA, const LineSegment& segB)
{
	const glm::vec3 d0 = segA.end - segA.start; // 線分Aの方向ベクトル
	const glm::vec3 d1 = segB.end - segB.start; // 線分Bの方向ベクトル
	const glm::vec3 r = segA.start - segB.start;
	const float a = glm::dot(d0, d0); // 線分Aの長さの2乗
	const float b = glm::dot(d0, d1);
	const float c = glm::dot(d0, r);
	const float e = glm::dot(d1, d1); // 線分Bの長さの2乗
	const float f = glm::dot(d1, r);

	// 線分の長さが0という特殊ケースを処理する
	// 長さが0の線分を「点」とみなして計算

	// 線分AとBの両方の長さが0
	if (a < 0.0001f && e < 0.0001f) {
		return { segA.start, segB.start };
	}

	// 線分Aの長さが0
	if (a < 0.0001f) {
		if (f <= 0) {
			return { segA.start, segB.start };
		}
		else if (f >= e) {
			return { segA.start, segB.end };
		}
		return { segA.start, segB.start + d1 * (f / e) };
	}

	// 線分Bの長さが0
	if (e < 0.0001f) {
		if (-c <= 0) {
			return { segA.start, segB.start };
		}
		else if (-c >= a) {
			return { segA.end, segB.start };
		}
		return { segA.start + d0 * (-c / a), segB.start };
	}

	// 線分の長さが0より大きいと分かったら、線分が平行かどうかをチェックする
	// 線分が平行な場合、sを求める式の分母が0になり計算できないため
	const float denom = a * e - b * b;
	if (denom) {
		// 線分Aの最近接点を求め、線分Aの範囲に制限
		float s = std::clamp((f * b - c * e) / denom, 0.0f, 1.0f);

		// 線分Bの最近接点を求める
		float t = b * s + f;

		// tが線分Bの外側を指している場合
		// tを線分Bの端点に設定し、sを計算しなおす
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

	// 線分が平行な場合、線分の重複する範囲の中点を選択

	// 線分Bを線分Aに射影
	float u = glm::dot(d0, segB.start - segA.start);
	float v = glm::dot(d0, segB.end - segA.start);

	// 射影した座標を線分Aの範囲に制限
	u = std::clamp(u, 0.0f, a);
	v = std::clamp(v, 0.0f, a);

	// uとvの中点が線分Aの最近接点になる
	float s = (u + v) / a * 0.5f;
	LineSegment result;
	result.start = segA.start + d0 * s;

	// 線分Aの最近接点を線分Bに射影
	const float t = dot(d1, result.start - segB.start);

	// 射影した座標を線分Bの範囲に制限
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
	// 線分の始点を面に射影
	const glm::vec3 ca = seg.start - rect.center;
	const float a0 = glm::dot(ca, rect.axis[0]);
	const float a1 = glm::dot(ca, rect.axis[1]);

	// 長方形と線分の平行性を調べる
	const glm::vec3 ab = seg.end - seg.start;
	const float n = glm::dot(rect.normal, ab);
	if (abs(n) <= 0.0001f) {
		// 平行な場合、線分が長方形の上に乗っているかどうかを調べる
		float tmin = -FLT_MAX;
		float tmax = FLT_MAX;
		const float list[] = { a0, a1 };
		for (int i = 0; i < 2; ++i) {
			// 線分とaxis[i]との平行性を調べる
			const float d = glm::dot(ab, rect.axis[i]);
			if (abs(d) <= 0.0001f) {
				continue; // 平行なので交点がない
			}
			// 長方形中心からの距離を線分の比率として表す
			float t0 = (-rect.scale[i] - list[i]) / d;
			float t1 = (rect.scale[i] - list[i]) / d;
			if (t0 > t1) {
				std::swap(t0, t1);
			}
			// 長方形と交差している範囲を計算
			tmin = (std::max)(t0, tmin);
			tmax = (std::min)(t1, tmax);
		}

		// 線分の始点と終点の範囲に制限する
		tmin = (std::max)(0.0f, tmin);
		tmax = (std::min)(1.0f, tmax);

		// 線分が長方形に重なっている場合
		// 重複範囲の中心を最近接点とする
		if (tmin <= tmax) {
			const glm::vec3 p = seg.start + ab * ((tmin + tmax) * 0.5f);
			const float d = dot(rect.normal, p - rect.center);
			return { p, p - rect.normal * d };
		}
	} // if (abs(n) <= 0.0001f)

	// 線分が面と交差しているかチェック
	// 交差している場合、最近接点は交点になる
	{
		const float d = glm::dot(rect.normal, rect.center);
		const float t = (d - glm::dot(rect.normal, seg.start)) / n;
		if (0 <= t && t <= 1) {
			// 交点が長方形の範囲内にあるか調べる
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

	// 平行でなく、交差もしていない場合

	// 線分の終点を面に射影
	const glm::vec3 cb = seg.end - rect.center;
	const float b0 = glm::dot(cb, rect.axis[0]);
	const float b1 = glm::dot(cb, rect.axis[1]);

	// 線分の始点および終点の最近接点を求め
	// 長方形の範囲に含まれる点のうちより近いほうを「仮の最近接点」とする
	float distance = FLT_MAX;
	LineSegment result;
	if (abs(a0) <= rect.scale[0] && abs(a1) <= rect.scale[1]) {
		distance = dot(rect.normal, ca);
		distance *= distance; // 比較のために2乗する
		result = { seg.start, rect.center + rect.axis[0] * a0 + rect.axis[1] * a1 };
	}
	if (abs(b0) <= rect.scale[0] && abs(b1) <= rect.scale[1]) {
		float tmp = dot(rect.normal, cb);
		tmp *= tmp; // 比較のために2乗する
		if (tmp < distance) {
			distance = tmp;
			result = { seg.end, rect.center + rect.axis[0] * b0 + rect.axis[1] * b1 };
		}
	}

	// 長方形の4辺について、最近接点を計算
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
		const float distanceEdge = glm::dot(ab, ab); // 長さの2乗
		if (distanceEdge < distance) { // 2乗同士の比較
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
	// 2つのOBBを軸に射影
	float minA, maxA, minB, maxB;
	ProjectBoxToAxis(verticesA, axis, minA, maxA);
	ProjectBoxToAxis(verticesB, axis, minB, maxB);

	// 射影した範囲が交差していないなら0を返す
	if (minB > maxA && minA > maxB) {
		return 0;
	}

	// 衝突法線は「OBBその1」が左、「OBBその2」が右にあると想定しているので、
	// 順序が逆になっている場合、衝突法施の向きを反転する必要がある、
	// ということを、呼び出し元に伝える
	shouldFlip = minB < minA;

	// 交差している距離を計算
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
		// X軸方向
		{ vertices[0], vertices[1] },
		{ vertices[2], vertices[3] },
		{ vertices[4], vertices[5] },
		{ vertices[6], vertices[7] },

		// Y軸方向
		{ vertices[0], vertices[2] },
		{ vertices[1], vertices[3] },
		{ vertices[4], vertices[6] },
		{ vertices[5], vertices[7] },

		// Z軸方向
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
			return false; // 点はOBBの外側に有る
		}
	}
	return true; // 点はOBBの内側にある
}

bool ClipLineSegment(const LineSegment& seg, const Plane& plane, glm::vec3& point)
{
	// 辺ベクトルを面法線に射影
	const glm::vec3 ab = seg.end - seg.start;
	float n = glm::dot(plane.normal, ab);
	if (abs(n) < 0.0001f) {
		return false; // 平行なので交差なし
	}

	// 交点までの距離を求める
	float t = plane.distance - dot(plane.normal, seg.start);
	if (t < 0 || t > n) {
		return false; // 交点が線分の外にあるので交差なし
	}

	// 交点座標を計算
	t /= n; // ベクトルabに対する比率に変換
	point = seg.start + ab * t;
	return true;
}

std::vector<glm::vec3> ClipEdges(const std::vector<glm::vec3>& vertices, const Box& box)
{
	// 辺と面を取得
	const auto edges = GetEdges(vertices.data());
	const auto planes = GetPlanes(box);

	// 交点用のメモリを予約
	// 2つのOBBが相当深く交差していない限り、交点の数は辺の数以下になるはず
	std::vector<glm::vec3> result;
	result.reserve(edges.size());

	// 辺と面の交点を求める
	for (const auto& plane : planes) {
		for (const auto& edge : edges) {
			glm::vec3 p;
			if (ClipLineSegment(edge, plane, p)) {
				// 交点がOBBその2の内側にあるなら、有効な交点として記録
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
	// カプセルを軸に射影
	float minA, maxA;
	minA = maxA = glm::dot(axis, capsule.seg.start);
	float f = glm::dot(axis, capsule.seg.end);
	minA = (std::min)(minA, f) - capsule.radius;
	maxA = (std::max)(maxA, f) + capsule.radius;

	// OBBを軸に射影
	minIndex = maxIndex = 0;
	float minB, maxB;
	minB = maxB = dot(axis, verticesB[0]);
	for (int i = 1; i < 8; ++i) {
		float f = dot(axis, verticesB[i]);
		if (f < minB) {
			minB = f;
			minIndex = i; // 最小値のインデックスを記録
		}
		if (f > maxB) {
			maxB = f;
			maxIndex = i; // 最大値のインデックスを記録
		}
	} // for

	 // 距離が0以上なら分離超平面を入れる隙間がある(つまり交差していない)
	if (minB > maxA && minA > maxB) {
		return false;
	}

	// 衝突法線は「OBBその1」が左、「OBBその2」が右にあると想定しているので、
	// 順序が逆になっている場合、衝突法施の向きを反転する必要がある、
	// ということを、呼び出し元に伝える
	shouldFlip = minB < minA;

	// 交差している距離を計算
	float lenA = maxA - minA;
	float lenB = maxB - minB;
	float min = std::min(minA, minB);
	float max = std::max(maxA, maxB);
	float length = max - min;
	return length - lenA - lenB;
}

bool Intersect(const Sphere& sphere, const AABB& aabb, ContactPoint& cp)
{
	// 最近接点から球体の中心までの距離が、球体の半径より大きければ衝突していない
	const glm::vec3 p = ClosestPoint(aabb, sphere.center); // 最近接点
	const glm::vec3 d = sphere.center - p; // 最近接点から球体の中心へのベクトル
	const float d2 = dot(d, d); // 距離の2乗
	if (d2 > sphere.radius * sphere.radius) {
		return false; // 衝突していない
	}
	if (d2 < 0.00001f) {
		// 距離が0の場合、球体の中心はAABBの内部にある
		// この場合、貫通距離が最も短い面から衝突したとみなす
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
		// 距離が0より大きい場合、球体の中心はAABBの外側にある
		// この場合、最近接から球体の中心へ向かう方向から衝突したとみなす
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
	// 最近接点から球体の中心までの距離が、球体の半径より大きければ衝突していない
	glm::vec3 p = ClosestPoint(box, sphere.center);
	glm::vec3 d = sphere.center - p;
	float d2 = dot(d, d);
	if (d2 > sphere.radius * sphere.radius) {
		return false;
	}

	if (d2 < 0.00001f) {
		// 距離が0の場合、球体の中心はOBBの内部にある
		// この場合、貫通距離が最も短い面から衝突したとみなす
		const glm::vec3 a = p - box.center; // OBB中心から球体中心へのベクトル
		cp.penetration = -FLT_MAX;
		for (int i = 0; i < 3; ++i) {
			float f = dot(box.axis[i], a); // 固有軸へ射影
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
		// 距離が0より大きい場合、球体の中心はOBBの外側にある
		// この場合、最近接から球体の中心へ向かう方向から衝突したとみなす
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
	// OBBの頂点座標配列を取得
	const auto verticesA = boxA.getVertice();
	const auto verticesB = boxB.getVertice();

	// 貫通距離を最大にしておく
	cp.penetration = -FLT_MAX;

	// 分離軸判定用の軸ベクトルを計算
	glm::vec3 axisList[15] = {
	  boxA.axis[0], boxA.axis[1], boxA.axis[2],
	  -boxB.axis[0], -boxB.axis[1], -boxB.axis[2],
	};
	for (int i = 0; i < 3; ++i) {
		axisList[6 + i * 3 + 0] = cross(axisList[i], axisList[3]);
		axisList[6 + i * 3 + 1] = cross(axisList[i], axisList[4]);
		axisList[6 + i * 3 + 2] = cross(axisList[i], axisList[5]);
	}

	// すべての軸ベクトルに対して分離軸判定を実行
	for (int i = 0; i < 15; ++i) {
		// 外積によって作られた軸ベクトルの場合、元の軸が平行だと長さが0になる
		// この場合、AまたはBの軸による分離軸判定と等しいのでスキップする
		const float length2 = dot(axisList[i], axisList[i]);
		if (length2 < 0.0001f) {
			continue;
		}

		// 交差している距離を計算
		bool shouldFlip;
		float d = GetIntersectionLength(
			verticesA.data(), verticesB.data(), axisList[i], shouldFlip);

		// 距離が0以上なら分離超平面を入れる隙間がある(つまり交差していない)
		if (d >= 0) {
			return false; // 衝突していない
		}

		// 外積で作成した軸ベクトルの長さは1にならない(sinθなので)
		// 結果として、dは軸ベクトルの長さを単位とした値になる
		// しかし、単位が異なると長さを比較できないため、正規化して単位を揃える
		d /= std::sqrt(length2);

		// 交差距離がより短い方向から衝突したとみなす
		if (d > cp.penetration) {
			cp.penetration = d;
			// 衝突法線は軸ベクトルの逆方向
			if (shouldFlip) {
				cp.normal = axisList[i];
			}
			else {
				cp.normal = -axisList[i];
			}
		}
	}

	// 衝突法線を正規化
	cp.normal = normalize(cp.normal);

	// コンタクトポイントを計算

 // OBBとOBBの交点を求める
	std::vector<glm::vec3> c0 = ClipEdges(verticesB, boxA);
	std::vector<glm::vec3> c1 = ClipEdges(verticesA, boxB);
	c0.insert(c0.end(), c1.begin(), c1.end());

	// 貫通距離の中間に衝突平面を設定(衝突平面上の点pを求める)
	float min, max;
	ProjectBoxToAxis(verticesA.data(), cp.normal, min, max);
	const glm::vec3 p = boxA.center - cp.normal * (max - min + cp.penetration) * 0.5f;

	// 交点を衝突平面に射影しつつ、重複する交点を削除
	int count = 0;
	for (int i = 0; i < c0.size(); ++i, ++count) {
		// 交点を衝突平面に射影
		c0[count] = c0[i] + cp.normal * dot(cp.normal, p - c0[i]);

		// 重複する交点がある場合は削除
		for (int j = 0; j < count; ++j) {
			const glm::vec3 v = c0[j] - c0[count];
			if (glm::dot(v, v) < 0.0001f) {
				--count;
				break;
			}
		}
	}
	c0.resize(count);

	// 交点の重心を計算し、コンタクトポイントとする
	cp.position = glm::vec3(0);
	for (const auto& e : c0) {
		cp.position += e;
	}
	cp.position /= static_cast<float>(c0.size());

	return true; // 衝突している
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

	// 最近接点同士の距離が0より大きい場合
	// 球体同士と同じ方法でコンタクトポイントを計算する
	if (d2 >= 0.00001f) {
		const float distance = std::sqrt(d2);
		cp.normal = d * (1.0f / distance);
		cp.penetration = distance - totalRadius;
		cp.position = seg.start - cp.normal * (capA.radius - cp.penetration * 0.5f);
		return true;
	}

	// 最近接点同士の距離が0の場合
	// segから法線を計算できないので、2軸に垂直な方向を衝突法線とする
	const glm::vec3 d0 = capA.seg.end - capA.seg.start;
	const glm::vec3 d1 = capB.seg.end - capB.seg.start;
	cp.normal = glm::cross(d0, d1);
	if (dot(cp.normal, cp.normal) < 0.00001f) {
		// 外積によって得た法線ベクトルの長さが0の場合、2軸は平行
		// 法線ベクトルを作れないので、d0ベクトルに垂直な方向を衝突法線とする
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

	// 大きい方の半径を貫通距離とする
	cp.penetration = -std::max(capA.radius, capB.radius);

	// コンタクトポイントの座標を計算
	cp.position = seg.start - cp.normal * (capA.radius - cp.penetration * 0.5f);

	return true;
}

bool Intersect(const Capsule& capsule, const Box& box, ContactPoint& cp)
{
	// OBBの頂点座標配列を取得
	const std::vector<glm::vec3> verticesA = { capsule.seg.start, capsule.seg.end };
	const auto verticesB = box.getVertice();

	// 分離軸判定用の軸ベクトルを計算
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

	// 貫通距離を最大にしておく
	cp.penetration = -FLT_MAX;

	// すべての軸ベクトルに対して分離軸判定を実行
	int axisIndex = 0; // 衝突した分離軸のインデックス
	int minIndex = 0, maxIndex = 0;
	for (int i = 0; i < std::size(axisList); ++i) {
		// 外積によって作られた軸ベクトルの場合、元の軸が平行だと長さが0になる
		// この場合、AまたはBの軸による分離軸判定と等しいのでスキップする
		const float length2 = dot(axisList[i], axisList[i]);
		if (length2 < 0.0001f) {
			continue;
		}

		// 分離軸を正規化
		const glm::vec3 axis = axisList[i] * (1.0f / sqrt(length2));

		// 交差している距離を計算
		bool shouldFlip;
		int tmpMinIndex, tmpMaxIndex;
		const float d = GetIntersectionLength(capsule, verticesB.data(),
			axis, shouldFlip, tmpMinIndex, tmpMaxIndex);

		// 距離が0以上なら分離超平面を入れる隙間がある(つまり交差していない)
		if (d >= 0) {
			return false;
		}

		// 貫通距離がより短い方向から衝突したとみなす
		if (d > cp.penetration) {
			cp.penetration = d;
			// 衝突法線は軸ベクトルの逆方向
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

	 // 半球部分の交差を確認
	const glm::vec3 list[] = { capsule.seg.start, capsule.seg.end };
	float direction = 1; // カプセル軸の向き調整用
	for (const auto& e : list) {
		// OBBの最近接点が半球側にない場合は無視
		const glm::vec3 c = ClosestPoint(box, e);
		const glm::vec3 v = c - e;
		const float p = glm::dot(capsuleDirection, v) * direction;
		direction *= -1;
		if (p >= 0) {
			continue;
		}
		// 最近接点までの距離がカプセルの半径より大きい場合は無視
		float t = glm::dot(v, v); // 距離の2乗
		if (t > capsule.radius * capsule.radius) {
			continue;
		}
		// 最近接点までの距離が分離軸による距離より短い場合のみ半球との衝突を選択
		t = sqrt(t); // 2乗の状態を解除
		const float d = t - capsule.radius;
		if (d > cp.penetration) {
			cp.penetration = d;
			cp.normal = -v * (1.0f / t);
			cp.position = c + cp.normal * (cp.penetration * 0.5f);
			axisIndex = -1;
		}
	} // for

	// コンタクトポイントを計算
	// 分離軸によって場合分け
	switch (axisIndex) {
	case 0:
	case 1:
	case 2: {
		// 面で衝突
		// 線分の始点および終点について、面上の最近接点を計算
		// 最近接点がOBBの範囲内にある場合のみ有効
		const int axisIndex1 = (axisIndex + 1) % 3;
		const int axisIndex2 = (axisIndex + 2) % 3;
		const Rectangle rect = {
		  box.center + cp.normal * box.extent[axisIndex],
		  cp.normal,
		  box.axis[axisIndex1], box.axis[axisIndex2],
		  box.extent[axisIndex1], box.extent[axisIndex2] };
		const auto seg = ClosestPoint(rect, capsule.seg);
		const glm::vec3 v = seg.start - seg.end;
		// 線分として扱える長さがあれば、線分から衝突法線を求める
		// ほぼ点なら分離超平面の法線を利用する
		if (glm::dot(v, v) >= 0.0001f) {
			cp.normal = normalize(v);
		}
		cp.position = seg.end + cp.normal * (cp.penetration * 0.5f);
		break;
	}
	case 3:
	case 4:
	case 5: {
		// 辺で衝突
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
		// 頂点で衝突
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