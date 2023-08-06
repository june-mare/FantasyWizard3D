#include "DrawCollision.h"

void VisualCollision::SetUp(std::array<glm::vec3, 8> vertices, std::array<unsigned int, 36> indices)
{
	vbo = BufferObject::Create(vertices.size() * sizeof(glm::vec3), &vertices);
	ibo = BufferObject::Create(indices.size() * sizeof(unsigned int), &indices);
	
	indicesSize = indices.size();
	
	vao = VertexArray::Create();
	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);

	/*有効にする頂点属性の配列インデックス*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindVertexArray(0);
}

void VisualCollision::Draw(ShaderPtr& shader,const glm::mat4& model,glm::vec4 color)
{
	shader->Use();
	shader->SetMat4("model", model);
	shader->SetFloat("color", color);
	glBindVertexArray(*vao);

	glDrawElements(GL_TRIANGLES,
		static_cast<unsigned int>(indicesSize), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);	
}
