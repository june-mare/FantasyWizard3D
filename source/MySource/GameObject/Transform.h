#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

float calcAngleBetweenVectors(const glm::vec3& vector1,const glm::vec3& vector2);

class Transform
{
protected:
	//ローカル情報
	glm::vec3 position = { 0.0f,0.0f,0.0f };
	glm::vec3 eulerRot = { 0.0f,0.0f,0.0f };//回転軸
	glm::vec3 scale = { 1.0f,1.0f,1.0f };
	glm::vec3 forward = { 0.0f,0.0f,1.0f };
	//グローバル情報用のマトリクス
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	//スケールがいじられたかどうか
	bool isDirty = false;

	glm::vec3 rotateAroundY(float angle);

public:
	enum Vector {
		up,
		right,
		front,
	};

	glm::mat4 getLocalModelMatrix();

	void lookAt(const glm::vec3 target, Vector type);

	void computeModelMatrix();

	//override 親モデルのマトリクスぶん回す
	void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix);

	void setLocalPosition(const glm::vec3& newPosition);

	void setLocalRotation(const glm::vec3& newRotation);

	void setLocalScale(const glm::vec3& newScale);

	glm::mat3 getRotationMat()const;

	glm::vec3 getGlobalPosition()const;

	const glm::vec3& getLocalPosition()const;

	const glm::vec3& getLocalRotation()const;

	const glm::vec3& getLocalScale()const;

	const glm::mat4& getModelMatrix()const;

	glm::vec3 getRight()const;

	glm::vec3 getUp()const;

	glm::vec3 getBackward()const;

	glm::vec3 getForward()const;

	glm::vec3 getGlobalScale()const;

	bool IsDirty()const;

};