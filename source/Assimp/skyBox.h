#pragma once
#ifndef ASSINPSKYBOX_H_INCLUDE
#define ASSINPSKYBOX_H_INCLUDE
#include <vector>
#include <string>
#include "../MySource/Shader/Shader.h"

class Camera;
class SkyBox
{
private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint texture = 0;
	Shader shader;
public:
	std::vector<std::string>
		textureNames;
	void Debug()noexcept;
	SkyBox(const char* vert, const char* frag);
	~SkyBox();
	void Draw(Camera& camera);
	void SetUp(std::string name = "");
};

#endif // !ASSINPSKYBOX_H_INCLUDE