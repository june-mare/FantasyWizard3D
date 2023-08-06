#include <glad/glad.h>
#include <memory>
#include <vector>
#include "2DImage/Texture.h"

//��s�錾
class FramebufferObject;
using FramebufferObjectPtr = std::shared_ptr<FramebufferObject>;
using FramebufferObjectList = std::vector<FramebufferObjectPtr>;

enum class FboType
{
	color = 1, // �J���[�e�N�X�`������
	depth = 2, // �[�x�e�N�X�`������
	colorDepth = color | depth, // �J���[�e�N�X�`���Ɛ[�x�e�N�X�`��
};

/**
* FboType���m�̘_����
*/
constexpr int operator&(FboType a, FboType b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}

class FramebufferObject
{
public:
	// FBO���쐬
	static FramebufferObjectPtr Create(int width, int height, FboType type);

	// �R���X�g���N�^�E�f�X�g���N�^
	FramebufferObject(int width, int height, FboType type);
	~FramebufferObject();

	// �R�s�[�Ƒ�����֎~
	FramebufferObject(const FramebufferObject&) = delete;
	FramebufferObject& operator=(const FramebufferObject&) = delete;

	// �t���[���o�b�t�@�̊Ǘ��ԍ����擾
	operator GLuint() const { return fbo; }

	// �e�N�X�`���̊Ǘ��ԍ����擾
	const TexturePtr& GetColorTexture() const { return texColor; }
	const TexturePtr& GetDepthTexture() const { return texDepth; }
	std::string name = "";
private:
	GLuint fbo = 0; // �t���[���o�b�t�@�I�u�W�F�N�g��ID
	TexturePtr texColor; // �J���[�e�N�X�`��
	TexturePtr texDepth; // �[�x�e�N�X�`��
};