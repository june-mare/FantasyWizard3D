#pragma once
#include "Window/GLFWWindow.h"
#include <string>

namespace InputSystem
{
	void Initialize(const char* path)noexcept;
	bool GetKey(int value)noexcept;
	bool GetMouseButton(int value)noexcept;
	bool GetKey(std::string action)noexcept;
	bool GetMouseButton(std::string action)noexcept;
	void DefaultInputList()noexcept;	
	void SaveFile(const char* path)noexcept;
	void DebugTextOut(const char* path)noexcept;
	bool AnyKeyDown()noexcept;
	std::string GetAnyKeyName()noexcept;
	int GetAnyKeyCode()noexcept;
	void RegisterInputInfo(std::string action)noexcept;
	void Finalize()noexcept;
};