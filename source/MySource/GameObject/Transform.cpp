#include "Transform.h"
#include "glm/gtx/transform.hpp"


glm::vec3 Transform::rotateAroundY(float angle)
{
	float angleRad = glm::radians(abs(angle)); // Šp“x‚ðâ‘Î’l‚É•ÏŠ·‚µAƒ‰ƒWƒAƒ“‚É•ÏŠ·
	float sinAngle = sin(angleRad);
	float cosAngle = cos(angleRad);

	float direction = glm::sign(angle); // Šp“x‚Ì•„†‚ðŽæ“¾‚µ‚Ä‰ñ“]•ûŒü‚ðŒˆ’è

	glm::mat3 rotationMatrix = glm::mat3(
		cosAngle, 0.0, direction * sinAngle,
		0.0, 1.0, 0.0,
		-direction * sinAngle, 0.0, cosAngle
	);

	return rotationMatrix * this->forward;
}

glm::mat4 Transform::getLocalModelMatrix()
{

	const glm::mat4 transformX =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);
	const glm::mat4 transformY =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	const glm::mat4 transformZ =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

	const glm::mat4 roationMatrix = transformY * transformX * transformZ;

	return glm::translate(glm::mat4(1.0f), position) * roationMatrix * glm::scale(glm::mat4(1.0f), scale);

}

void Transform::computeModelMatrix()
{
	modelMatrix = getLocalModelMatrix();
	isDirty = false;
}

void Transform::lookAt(const glm::vec3 target, Vector type)
{
	glm::vec3 direction = glm::normalize(target - position);
	float angle = calcAngleBetweenVectors(forward, direction);
	
	switch (type)
	{
	case Transform::up:
	eulerRot.y = angle;
		break;
	case Transform::right:
	eulerRot.x = angle;
		break;
	case Transform::front:
	eulerRot.z = angle;
		break;
	}
}

void Transform::computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
{
	modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
	isDirty = false;
}

void Transform::setLocalPosition(const glm::vec3& newPosition)
{
	position = newPosition;
	this->isDirty = true;
}

void Transform::setLocalRotation(const glm::vec3& newRotation)
{
	eulerRot = newRotation;
	isDirty = true;
}

void Transform::setLocalScale(const glm::vec3& newScale)
{
	scale = newScale;
	isDirty = true;
}

glm::mat3 Transform::getRotationMat()const
{
	const glm::mat3 transformX =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);
	const glm::mat3 transformY =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	const glm::mat3 transformZ =
		glm::rotate(
			glm::mat4(1.0f),
			glm::radians(eulerRot.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

	return transformY * transformX * transformZ;
}

glm::vec3 Transform::getGlobalPosition() const
{
	return glm::vec3(modelMatrix[3]);
}

const glm::vec3& Transform::getLocalPosition() const
{
	return position;
}

const glm::vec3& Transform::getLocalRotation() const
{
	return eulerRot;
}

const glm::vec3& Transform::getLocalScale() const
{
	return scale;
}

const glm::mat4& Transform::getModelMatrix() const
{
	return modelMatrix;
}

glm::vec3 Transform::getRight() const
{
	return modelMatrix[0];
}

glm::vec3 Transform::getUp() const
{
	return modelMatrix[1];
}

glm::vec3 Transform::getBackward() const
{
	return modelMatrix[2];
}

glm::vec3 Transform::getForward() const
{
	return -modelMatrix[2];
}

glm::vec3 Transform::getGlobalScale() const
{
	return { glm::length(getRight()),glm::length(getUp()),glm::length(getBackward()) };
}

bool Transform::IsDirty() const
{
	return isDirty;
}

float calcAngleBetweenVectors(const glm::vec3& vector1, const glm::vec3& vector2)
{
	glm::vec3 normalizedVector1 = glm::normalize(vector1);
	glm::vec3 normalizedVector2 = glm::normalize(vector2);
	float dotProduct = glm::dot(normalizedVector1, normalizedVector2);

	float angleInRadians = glm::acos(dotProduct);

	glm::vec3 crossProduct = glm::cross(normalizedVector1, normalizedVector2);
	if (glm::dot(crossProduct, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f){
		angleInRadians = -angleInRadians;
	}

	float angleInDegrees = glm::degrees(angleInRadians);
	return angleInDegrees;
}
