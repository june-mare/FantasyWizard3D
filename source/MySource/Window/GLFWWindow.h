#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#pragma comment(lib,"opengl32.lib")

namespace GLFW_WINDOW {

	class Window {
	public:
		static Window& Instance()
		{
			static Window instance;
			return instance;
		}
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;
		GLFWwindow* GetGLWindow() { return window; }
		
	private:
		Window();
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool isGLFWInitialized;
		bool isInitialized;

		GLFWwindow* window;
	};

}
