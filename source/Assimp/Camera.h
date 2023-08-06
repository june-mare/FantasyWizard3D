#pragma once
#include <glad/glad.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <string>

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
    :public std::enable_shared_from_this<Camera>
{
private:
    std::string name = {""};
    float Speed = 2.5f;
    float horizontalAngle = 90.f;
    float oldMouseOffsetX = 0.f;
    float radius = 7.f;
  
public:
    bool isDirty = false;
    bool isActive = false;
    bool isTarget = false;
    bool isLook = false;

    // アトリビュート
    glm::vec3 targetPos{};
    glm::vec3 enemyPos{};
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    glm::mat4 projection;
    glm::mat4 view;

    //角度
    float Yaw = -90.0f;
    float Pitch = 0.0f;
    // カメラオプション
    float MovementSpeed = 90.f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;
    float Far = 100.f;
    float Near = 0.1f;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = 0, float pitch = 0);
        
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
        
    glm::mat4 GetViewMatrix()noexcept;
   
    glm::mat4 GetProjection()noexcept;
    
    void LookAtPosition(glm::vec3 position);

    void SetOldMouseX(float x);

    void ProcessKeyboard(Camera_Movement direction)noexcept;
    
    void ProcessKeyboardVector(Camera_Movement direction)noexcept;

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
   
    void ProcessMouseScroll(float yoffset);
    inline glm::vec3 getPosition()noexcept{
        return Position;
    }
    inline glm::mat4 getView()noexcept {
        return view;
    }
    inline glm::mat4 getProjection()noexcept {
        return projection;
    }
    void UpdateCameraVectors();
    bool UpdateDatas();
    inline float getSpeed()noexcept{
        return Speed;
    }
    inline void setSpeed(float speed)noexcept{
        Speed = speed;
    }
    inline std::string getName()const noexcept{
        return name;
    }
    inline void setName(std::string name)noexcept{
        this->name = name;
    }
    inline float getRadius()const {
        return radius;
    }
    inline void setRadius(float radius) {
        this->radius = radius;
    }

};