#include "FrameBufferObject.h"
#include <iostream>
#include "Debug/Debug.h"

FramebufferObjectPtr FramebufferObject::Create(int width, int height, FboType type)
{
	return std::make_shared<FramebufferObject>(width, height, type);
}

FramebufferObject::FramebufferObject(int width, int height, FboType type)
{
	// �J���[�e�N�X�`�����쐬
	if (type & FboType::color) {
		texColor = Texture::Create("FBO(Color)", width, height, GL_RGBA16F, GL_LINEAR);
		if (!texColor || !*texColor) {
			LOG_ERROR("FBO�p�J���[�e�N�X�`���̍쐬�Ɏ��s");
			texColor.reset(); // �J���[�e�N�X�`����j��
			return;
		}

	}
	// �[�x�e�N�X�`�����쐬
	if (type & FboType::depth) {
		texDepth = Texture::Create("FBO(Depth)", width, height, GL_DEPTH_COMPONENT32F, GL_LINEAR,Texture::Type::depth);
		if (!texDepth || !*texDepth) {
			LOG_ERROR("FBO�p�[�x�e�N�X�`���̍쐬�Ɏ��s");
			texColor.reset(); // �J���[�e�N�X�`����j��
			texDepth.reset(); // �[�x�e�N�X�`����j��
			return;
		}
	}

	// FBO���쐬
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
	// FBO���쐬�ł������`�F�b�N
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("FBO�̍쐬�Ɏ��s");
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
		texColor.reset();
		texDepth.reset();
		return;
	}

	// �쐬����
	//LOG("FBO(width = %d height = %d ���쐬",width,height);
}

FramebufferObject::~FramebufferObject()
{
	glDeleteFramebuffers(1, &fbo);
}

