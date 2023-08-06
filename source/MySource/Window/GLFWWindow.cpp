#include"GLFWWindow.h"
#include <iomanip>
#include <iostream>
#include "../stbi/stb_image.h"

namespace GLFW_WINDOW {
	namespace
		/* unnamed */
	{
		/*GLFWからのエラー報告を処理する*/
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

	/*GLFWの初期化*/
	bool Window::Init(int w, int h, const char* title)
	{
		/*すでに作られているかの確認*/
		if (isInitialized) {
			::std::cerr << "ERROR: GLFWEWは既に初期化されています." << ::std::endl;
			return false;
		}

		/*ゲームウィンドウアイコンの設定*/


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
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // ウィンドウを非表示にする
			window = glfwCreateWindow(w, h, title, glfwGetPrimaryMonitor(), nullptr); //フルスクリーン
			//window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window) {
				stbi_image_free(image);
				return false;
			}
			glfwSetWindowIcon(window, 1, &icon);

			glfwShowWindow(window); // ウィンドウを表示する
			
			stbi_image_free(image);
		}
		else {
			stbi_image_free(image);
			return false;
		}

		/*描画ウィンドウの作成*/
			/*コンテキストの生成*/
		glfwMakeContextCurrent(window);
		
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		isInitialized = true;
		return true;
	}

	/*GLFWの終了のbool*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

	/* フロントバッファとバックバッファを切り替える*/
	void Window::SwapBuffers() const
	{
		glfwSwapBuffers(window);
	}

} // namespace GLFWEW
