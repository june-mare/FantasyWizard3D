#include "CameraBufferObject.h"
#include <glm/gtc/type_ptr.hpp>

CameraBufferObjectPtr CameraBufferObject::Create(GLsizeiptr size)
{
    return std::make_shared<CameraBufferObject>(size);
}

CameraBufferObject::CameraBufferObject(GLsizeiptr size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraBufferObject::SetBindBuffer(glm::mat4& projection, glm::mat4& view)
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
