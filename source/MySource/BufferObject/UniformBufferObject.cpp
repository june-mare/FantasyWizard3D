#include "UniformBufferObject.h"

UniformBufferObjectPtr UniformBufferObject::Create(size_t size, const void* data, GLbitfield flags)
{
    return std::make_shared<UniformBufferObject>(size, data, flags); 
}

UniformBufferObject::UniformBufferObject(size_t size, const void* data, GLbitfield flags)
{
    glCreateBuffers(1, &id);
    glNamedBufferData(id, size, data, flags);
    glBindBuffer(GL_UNIFORM_BUFFER, id);

}
