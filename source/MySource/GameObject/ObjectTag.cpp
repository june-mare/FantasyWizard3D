#include "ObjectTag.h"

namespace TAG
{
	namespace
	{
		std::vector<std::string> tags;
		char debugText[100] = { "" };
	}

	void Initialize(const char* path) noexcept
	{
		ReadFile(path);
	}

	std::string CheckforResistation(std::string tag) noexcept
	{
		for (auto itr : tags)
		{
			if (itr == tag)
			{
				return itr;
			}
		}
		return "untag";
	}

	void ReadFile(const char* path) noexcept
	{
		FileSystem::ReadFile(path, "tag", tags);
	}

	void SaveFile(const char* path) noexcept
	{
		FileSystem::SaveFile(path, "tag", tags);
	}

	std::vector<std::string> GetTags() noexcept
	{
		return tags;
	}

	void DebugTextOut(const char* path)
	{
		ImGui::Begin("Tag");

		if (ImGui::CollapsingHeader("Tags"))
		{
			std::vector<std::string>::iterator deleteData = tags.end();
			for (auto itr : tags)
			{
				if (ImGui::CollapsingHeader(itr.c_str()))
				{

				ImGui::Text(itr.c_str());
				if (ImGui::Button("DeleteThisTag", ImVec2(200.f, 30.f)))
				{
					deleteData = std::find(tags.begin(), tags.end(), itr);
				}
				ImGui::Separator();
				}
			}
			if (deleteData != tags.end())
			{
				tags.erase(deleteData);
				SaveFile(path);
			}

		}
		if (ImGui::CollapsingHeader("RegistTag"))
		{			
			ImGui::InputText("TagName", debugText, 100);
			if (ImGui::Button("Registation", ImVec2(200.f, 30.f)))
			{
				tags.push_back(debugText);
				SaveFile(path);
			}
		}

		ImGui::End();
	}

	void Finalize() noexcept
	{
		tags.clear();
	}
}