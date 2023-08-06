#include "Camera.h"
#include "../MySource/Time/Time.h"
#include "../MySource/Window/Viewport.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
	Position.x = targetPos.x + radius * cos(glm::radians(horizontalAngle));
	Position.z = targetPos.z + radius * sin(glm::radians(horizontalAngle));
	isDirty = true;
	UpdateDatas();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
	Position.x = targetPos.x + radius * cos(glm::radians(horizontalAngle));
	Position.z = targetPos.z + radius * sin(glm::radians(horizontalAngle));
	isDirty = true;
	UpdateDatas();
}

glm::mat4 Camera::GetViewMatrix() noexcept
{
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjection() noexcept
{
	Viewport& viewport = Viewport::Instance();
	return  glm::perspective(glm::radians(Zoom),
		viewport.GetWindowAspect(), this->Near, this->Far);
}

void Camera::LookAtPosition(glm::vec3 position)
{
	targetPos = position;
	targetPos.y += 1.f;
	Position.x = targetPos.x + radius * cos(glm::radians(horizontalAngle));
	Position.z = targetPos.z + radius * sin(glm::radians(horizontalAngle));
	glm::vec3 cameraDirection = glm::normalize(targetPos - Position);
	Front = cameraDirection;
	UpdateCameraVectors();
	isDirty = true;
}

void Camera::SetOldMouseX(float x)
{
	oldMouseOffsetX = x;
}

void Camera::ProcessKeyboard(Camera_Movement direction) noexcept
{
	isDirty = true;
	float velocity = MovementSpeed * Time::deltaTime;
	if (direction == Camera_Movement::FORWARD)
		Position += Front * velocity;
	if (direction == Camera_Movement::BACKWARD)
		Position -= Front * velocity;
	if (direction == Camera_Movement::LEFT)
		Position -= Right * velocity;
	if (direction == Camera_Movement::RIGHT)
		Position += Right * velocity;
}

void Camera::ProcessKeyboardVector(Camera_Movement direction) noexcept
{
	isDirty = true;
	if (direction == Camera_Movement::FORWARD)
		Yaw -= getSpeed() * Time::deltaTime;
	if (direction == Camera_Movement::BACKWARD)
		Yaw += getSpeed() * Time::deltaTime;
	if (direction == Camera_Movement::LEFT)
		Pitch += getSpeed() * Time::deltaTime;
	if (direction == Camera_Movement::RIGHT)
		Pitch -= getSpeed() * Time::deltaTime;
	UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (isLook)return;

	float result = xoffset - oldMouseOffsetX;
	if (std::signbit(result)) {
		horizontalAngle += (MovementSpeed * Time::deltaTime);
	}
	else {
		horizontalAngle -= (MovementSpeed * Time::deltaTime);
	}

	if (horizontalAngle >= 360.0f) {
		horizontalAngle -= 360.0f;
	}
	else if (horizontalAngle < 0.0f) {
		horizontalAngle += 360.0f;
	}
	
	oldMouseOffsetX = xoffset;	
	Position.x = targetPos.x + radius * cos(glm::radians(horizontalAngle));
	Position.z = targetPos.z + radius * sin(glm::radians(horizontalAngle));
	
	isDirty = true;
	// カメラの位置を適切な高さに設定する（例：y座標を適当な値に設定）
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
	projection = GetProjection();
}

void Camera::UpdateCameraVectors()
{
	//glm::vec3 front;
	/*front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));*/
	//Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	UpdateDatas();
}

bool Camera::UpdateDatas()
{
	if (!isDirty)return false;
	projection = GetProjection();
	view = GetViewMatrix();
	isDirty = false;
	return true;
}
