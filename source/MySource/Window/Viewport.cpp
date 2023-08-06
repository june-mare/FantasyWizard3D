#include "Viewport.h"
#include <glad/glad.h>

glm::vec4 Viewport::GetViewport() const noexcept
{
	return glm::vec4(this->x, this->y, this->width, this->height);
}

glm::vec2 Viewport::GetViewSize()noexcept
{
	return glm::vec2(static_cast<float>(GetViewSizeX()),
		static_cast<float>(GetViewSizeY()));
}

int Viewport::GetViewSizeX() noexcept
{
	return this->width - this->x;
}

int Viewport::GetViewSizeY() noexcept
{
	return this->height - this->y;
}

float Viewport::GetWindowAspect() noexcept
{
	return static_cast<float>(GetViewSizeX()) / static_cast<float>(GetViewSizeY());
}

void Viewport::ChangeScreenSize(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	glViewport(x, y, w, h);
}

void Viewport::LoadScreenSize() const noexcept
{
	glViewport(this->x, this->y, this->width, this->height);
}
