#include "FrameBufferObject.h"
#include <iostream>
#include "Debug/Debug.h"

FramebufferObjectPtr FramebufferObject::Create(int width, int height, FboType type)
{
	return std::make_shared<FramebufferObject>(width, height, type);
}

FramebufferObject::FramebufferObject(int width, int height, FboType type)
{
	// カラーテクスチャを作成
	if (type & FboType::color) {
		texColor = Texture::Create("FBO(Color)", width, height, GL_RGBA16F, GL_LINEAR);
		if (!texColor || !*texColor) {
			LOG_ERROR("FBO用カラーテクスチャの作成に失敗");
			texColor.reset(); // カラーテクスチャを破棄
			return;
		}

	}
	// 深度テクスチャを作成
	if (type & FboType::depth) {
		texDepth = Texture::Create("FBO(Depth)", width, height, GL_DEPTH_COMPONENT32F, GL_LINEAR,Texture::Type::depth);
		if (!texDepth || !*texDepth) {
			LOG_ERROR("FBO用深度テクスチャの作成に失敗");
			texColor.reset(); // カラーテクスチャを破棄
			texDepth.reset(); // 深度テクスチャを破棄
			return;
		}
	}

	// FBOを作成
	glCreateFramebuffers(1, &fbo);
	if (static_cast<int>(type) & 1) {
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, *texColor, 0);
	}
	else {
		glNamedFramebufferDrawBuffer(fbo, GL_NONE);	
	}
	if (static_cast<int>(type) & 2) {
		glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, *texDepth, 0);
	}
	// FBOが作成できたかチェック
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("FBOの作成に失敗");
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
		texColor.reset();
		texDepth.reset();
		return;
	}

	// 作成成功
	//LOG("FBO(width = %d height = %d を作成",width,height);
}

FramebufferObject::~FramebufferObject()
{
	glDeleteFramebuffers(1, &fbo);
}

