#include"GLFWWindow.h"
#include <iomanip>
#include <iostream>
#include "../stbi/stb_image.h"

namespace GLFW_WINDOW {
	namespace
		/* unnamed */
	{
		/*GLFW����̃G���[�񍐂���������*/
		void ErrorCallback(int error, const char* desc)
		{
			::std::cerr << "ERROR(0x" << ::std::hex << ::std::setfill('0') << ::std::setw(8) << error << "): " << desc << ::std::endl;
		}
	} // unnamed namespace

	Window::Window() : isGLFWInitialized(false), isInitialized(false), window(nullptr)
	{}
	Window::~Window()
	{
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}

	/*GLFW�̏�����*/
	bool Window::Init(int w, int h, const char* title)
	{
		/*���łɍ���Ă��邩�̊m�F*/
		if (isInitialized) {
			::std::cerr << "ERROR: GLFWEW�͊��ɏ���������Ă��܂�." << ::std::endl;
			return false;
		}

		/*�Q�[���E�B���h�E�A�C�R���̐ݒ�*/


		glfwInit();

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		glfwWindowHint(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		
		int width, height, channels;
		unsigned char* image = stbi_load("resource/textures/windowIcon.png", &width, &height, &channels, 0);
		if (image && !window) {
			GLFWimage icon;
			icon.width = width;
			icon.height = height;
			icon.pixels = image;
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // �E�B���h�E���\���ɂ���
			window = glfwCreateWindow(w, h, title, glfwGetPrimaryMonitor(), nullptr); //�t���X�N���[��
			//window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window) {
				stbi_image_free(image);
				return false;
			}
			glfwSetWindowIcon(window, 1, &icon);

			glfwShowWindow(window); // �E�B���h�E��\������
			
			stbi_image_free(image);
		}
		else {
			stbi_image_free(image);
			return false;
		}

		/*�`��E�B���h�E�̍쐬*/
			/*�R���e�L�X�g�̐���*/
		glfwMakeContextCurrent(window);
		
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		isInitialized = true;
		return true;
	}

	/*GLFW�̏I����bool*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

	/* �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���*/
	void Window::SwapBuffers() const
	{
		glfwSwapBuffers(window);
	}

} // namespace GLFWEW
