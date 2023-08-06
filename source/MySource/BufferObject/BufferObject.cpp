#include "BufferObject.h"
#include "Debug/Debug.h"
#include "Debug/GLDebug.h"

/**
* �o�b�t�@�I�u�W�F�N�g���쐬����
*
* @param type        �o�b�t�@�I�u�W�F�N�g�̎��
* @param size        �o�b�t�@�I�u�W�F�N�g�̃o�C�g�T�C�Y
* @param data        �o�b�t�@�ɃR�s�[����f�[�^�̃A�h���X
*                    �s�v�ȏꍇ��nullptr���w�肷��
* @param flags       �g�r�������r�b�g�t���O�̘_���a(glBufferStorage���Q��)
*
* @return �쐬�����o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
*/
BufferObjectPtr BufferObject::Create(
	GLsizeiptr size, const void* data, GLbitfield flags)
{
	return std::make_shared<BufferObject>(size, data, flags);
}

/**
* �R���X�g���N�^
*
* @param type        �o�b�t�@�I�u�W�F�N�g�̎��
* @param size        �o�b�t�@�I�u�W�F�N�g�̃o�C�g�T�C�Y
* @param data        �o�b�t�@�ɃR�s�[����f�[�^�̃A�h���X
*                    �s�v�ȏꍇ��nullptr���w�肷��
* @param flags       �g�r�������r�b�g�t���O�̘_���a(glBufferStorage���Q��)
*/
BufferObject::BufferObject(GLsizeiptr size, const void* data, GLbitfield flags)
{
	bufferSize = size;
	glCreateBuffers(1, &id);
	glNamedBufferStorage(id, bufferSize, data, flags);
}

BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &id);
}


/**
* �o�b�t�@�I�u�W�F�N�g���쐬����
*
* @param size        �o�b�t�@�I�u�W�F�N�g�̃o�C�g�T�C�Y
* @param type        �o�b�t�@�I�u�W�F�N�g�̎��
* @param access      �ȉ��̂����ꂩ���w�肷��
*                    GL_READ_WRITE �ǂݎ��Ə������݂̗��Ή��Ń}�b�v����
*                    GL_READ_ONLY  �ǂݎ���p�Ƃ��ă}�b�v����
*                    GL_WRITE_ONLY �������ݐ�p�Ƃ��ă}�b�v����
*
* @return �쐬�����o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
*/
MappedBufferObjectPtr MappedBufferObject::Create(GLsizeiptr size, GLenum type, GLenum access)
{
	return std::make_shared<MappedBufferObject>(size, type, access);
}

/**
* �R���X�g���N�^
*
* @param size        �o�b�t�@�I�u�W�F�N�g�̃o�C�g�T�C�Y
* @param type        �o�b�t�@�I�u�W�F�N�g�̎��
* @param access      �ȉ��̂����ꂩ���w�肷��
*                    GL_READ_WRITE �ǂݎ��Ə������݂̗��Ή��Ń}�b�v����
*                    GL_READ_ONLY  �ǂݎ���p�Ƃ��ă}�b�v����
*                    GL_WRITE_ONLY �������ݐ�p�Ƃ��ă}�b�v����
*/
MappedBufferObject::MappedBufferObject(GLsizeiptr size, GLenum type, GLenum access):
  type(type)
{
	// �_�u���o�b�t�@�̂��߁A�T�C�Y��256�o�C�g���E�ɌJ��グ��
	bufferSize = ((size + 255) / 256) * 256;

	// �A�N�Z�X�̎�ނɂ���ăt���O��I��
	GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	switch (access) {
	default:
		LOG_ERROR("�������Ԉ���Ă��܂�(access=%d)", access);
		[[fallthrough]];
	case GL_READ_WRITE: flags |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
	case GL_READ_ONLY:  flags |= GL_MAP_READ_BIT; break;
	case GL_WRITE_ONLY: flags |= GL_MAP_WRITE_BIT; break;
	}

	// �o�b�t�@�I�u�W�F�N�g���쐬
	glCreateBuffers(1, &id);
	CheckGLError("glCreateBuffers");
	glNamedBufferStorage(id, bufferSize * 2, // �_�u���o�b�t�@�p��2�{�m��
		nullptr, flags);
	CheckGLError("glNamedBufferStorage");

	//LOG("ShaderStorageBufferObject�쐬");

	// GPU��������CPU�������A�h���X�Ƀ}�b�v����
	p = static_cast<uint8_t*>(glMapNamedBuffer(id, access));
	CheckGLError("glMapNamedBuffer");
}

/**
* �f�X�g���N�^
*/
MappedBufferObject::~MappedBufferObject()
{
	glDeleteBuffers(1, &id);
}

/**
* GPU���o�b�t�@���g���I���̂�҂�
*/
void MappedBufferObject::WaitSync()
{
	for (GLsync& sync = syncList[bufferIndex]; sync;) {
		const GLuint64 timeout = 16'700'000; // 16.7�~���b(��1/60�b)
		const GLenum status = glClientWaitSync(sync, 0, timeout);
		if (status != GL_TIMEOUT_EXPIRED) {
			// �g���I����������I�u�W�F�N�g�͍폜���Ȃ��Ă͂Ȃ�Ȃ�
			glDeleteSync(sync);
			sync = 0;
			break;
		}
	} // for syncList
}

/**
* �f�[�^�̃R�s�[��A�h���X���擾����
*
* @return �R�s�[��A�h���X
*
* �R�s�[�\�Ȕ͈͂�GetSize()�Ŏ擾�ł���
*/
uint8_t* MappedBufferObject::GetMappedAddress() const
{
	return p + bufferIndex * bufferSize;
}

/**
* �w�肵���͈͂�OpenGL�R���e�L�X�g�Ɋ��蓖�Ă�
*
* @param index  ���蓖�Ă�o�C���f�B���O�|�C���g�ԍ�
* @param offset ���蓖�Ĕ͈͂̐擪�ʒu
* @param size   ���蓖�Ĕ͈͂̃o�C�g�T�C�Y
*/
void MappedBufferObject::Bind(
	GLuint index, size_t offset, size_t size)
{
	offset += bufferIndex * bufferSize;
	glBindBufferRange(type, index, id, offset, size);
}

/**
* �R�s�[��o�b�t�@��؂�ւ���
*/
void MappedBufferObject::SwapBuffers()
{
	syncList[bufferIndex] =
		glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	bufferIndex = (bufferIndex + 1) % 2;
}