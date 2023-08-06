#include "InputSystem.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "FileSystem.h"
#include "DataStruct/useImGui.h"

namespace InputSystem
{
	namespace
	{
		const int defaultInputNum = 7;
		const char* defaultKeys[] =
		{
		  "jump",
		  "front",
		  "left",
		  "back",
		  "right",
		  "atacck",
		  "lock",
		};
		const int defaultValues[] =
		{
		  GLFW_KEY_SPACE,
		  GLFW_KEY_W,
		  GLFW_KEY_A,
		  GLFW_KEY_S,
		  GLFW_KEY_D,
		  GLFW_KEY_LEFT_SHIFT,
		  GLFW_KEY_RIGHT_CONTROL,
		};
		char debugText[100] = { "" };
		std::unordered_map<std::string, int>
			commandList;
		std::unordered_map<int, std::string>
			inputList;
	}//unname !namespace

	void Initialize(const char* path) noexcept
	{
		FileSystem::ReadFile<int, std::string>(path,"InputList",inputList);
		FileSystem::ReadFile<std::string, int>(path,"CommandList",commandList);
	}

	bool GetKey(int value) noexcept
	{
		GLFW_WINDOW::Window& windows = GLFW_WINDOW::Window::Instance();
		return glfwGetKey(windows.GetGLWindow(), value);
	}

	bool GetMouseButton(int value) noexcept
	{
		GLFW_WINDOW::Window& windows = GLFW_WINDOW::Window::Instance();
		return glfwGetMouseButton(windows.GetGLWindow(), value);
	}


	bool GetKey(std::string action)noexcept
	{
		GLFW_WINDOW::Window& windows = GLFW_WINDOW::Window::Instance();
		if (commandList.find(action) != commandList.end())
			return glfwGetKey(windows.GetGLWindow(), commandList[action]);
		return false;
	}

	bool GetMouseButton(std::string action)noexcept
	{
		GLFW_WINDOW::Window& windows = GLFW_WINDOW::Window::Instance();
		if (commandList.find(action) != commandList.end())
			return glfwGetMouseButton(windows.GetGLWindow(), commandList[action]);
		return false;
	}


	void DefaultInputList()noexcept
	{
		for (int i = 0; i < defaultInputNum; i++)
		{
			commandList[defaultKeys[i]]
				= defaultValues[i];
		}
	}

	void SaveFile(const char* path)noexcept
	{
		FileSystem::SaveFile<std::string, int>(path, "CommandList", commandList);
	}

	void DebugTextOut(const char* path) noexcept
	{

		ImGui::Begin("InputSystem");
		int num = 0;
		if (ImGui::CollapsingHeader("CommandList"))
		{
			std::string deleteData = { "" };
			for (auto itr : commandList)
			{
				if (ImGui::CollapsingHeader(itr.first.c_str()))
				{

					ImGui::Text("NowCommandKey");
					ImGui::Text(inputList[itr.second].c_str());
					if (ImGui::Button("NewKeyDown&ButtonPush", ImVec2(200.f, 30.f)))
					{
						if (AnyKeyDown())
						{
							RegisterInputInfo(itr.first);
							SaveFile(path);
						}
					}

					if (ImGui::Button("DeleteThisCommand", ImVec2(200.f, 30.f)))
					{
						deleteData = itr.first;
					}
				}
				num++;
				ImGui::Separator();
			}
			if (deleteData != "")
			{
				commandList.erase(deleteData);
				SaveFile(path);
			}
		}
		ImGui::Separator();

		if (ImGui::CollapsingHeader("SetNewCommandKey"))
		{
			ImGui::InputText("CommandName", debugText, 100);
			if (ImGui::Button("KeyDown&ButtonPush", ImVec2(200.f, 30.f)))
			{
				if (InputSystem::AnyKeyDown())
				{
					RegisterInputInfo(debugText);
					SaveFile(path);
				}
			}
		}

		if (ImGui::CollapsingHeader("SetDefaultCommandKey"))
		{		
			if (ImGui::Button("ReturnDefault", ImVec2(200.f, 30.f)))
			{
				DefaultInputList();
				SaveFile(path);
			}
		}

		ImGui::End();
	}

	bool AnyKeyDown() noexcept
	{
		for (auto itr : inputList)
		{
			if (GetKey(itr.first))
				return true;
		}
		return false;
	}

	int GetAnyKeyCode() noexcept
	{
		for (auto itr : inputList)
		{
			if (GetKey(itr.first))
				return itr.first;
		}
		return NULL;
	}

	void RegisterInputInfo(std::string action) noexcept
	{
		commandList[action] = GetAnyKeyCode();
	}

	void Finalize() noexcept
	{
		commandList.clear();
		inputList.clear();
	}

	std::string GetAnyKeyName() noexcept
	{
		return inputList[GetAnyKeyCode()];
	}
};