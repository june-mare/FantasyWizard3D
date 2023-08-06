#include "Mesh.h"

namespace Mesh
{
	void Mesh::SetupMesh()
	{
		// create buffers/arrays
		vao = VertexArray::Create();
		glCreateBuffers(1, &vbo);
		glCreateBuffers(1, &ibo);

		glBindVertexArray(*vao);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitTangents));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
		:vertices(vertices), indices(indices), textures(textures) {
		SetupMesh();
	}

	void Mesh::Draw(Shader& shader, bool isInstance)const
	{
		if (!isInstance) {		
			for (int i = 0; i < textures.size(); i++){

				glActiveTexture(GL_TEXTURE0 + i);
				if (textures[i].typeId == Texture::diffuse) {
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
					glUniform1i(glGetUniformLocation(shader.id, "texture_diffuse1"), 0);
				}
				else if (textures[i].typeId == Texture::specular) {
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
					glUniform1i(glGetUniformLocation(shader.id, "texture_specular1"), 1);
				}
				else if (textures[i].typeId == Texture::normal) {
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
					glUniform1i(glGetUniformLocation(shader.id, "texture_normal1"), 2);
				}
			}
		}

		glBindVertexArray(*vao);		
	}

	void Mesh::Draw(Shader& shader, Material& material, bool isInstance) const
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number = "";
			std::string name = "";

			if (textures[i].typeId == Texture::diffuse) {
				number = std::to_string(diffuseNr++);
				name = "texture_diffuse";
			}
			else if (textures[i].typeId == Texture::specular) {
				number = std::to_string(specularNr++);
				name = "texture_specular";
			}
			else if (textures[i].typeId == Texture::normal) {
				number = std::to_string(normalNr++);
				name = "texture_normal";
			}
			else if (textures[i].typeId == Texture::height) {
				number = std::to_string(heightNr++);
				name = "texture_height";
			}

			glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);

			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glBindVertexArray(*vao);
	}

}