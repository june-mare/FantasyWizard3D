#include "TextWindow.h"
#include "useImGui.h"
#include "Time/Time.h"
#include "System/InputSystem.h"
#include <GLFW/glfw3.h>


unsigned int TextWindow::TextLineMax(int lenNum)noexcept
{
	if (count > length)
	{
		if (lenNum == 0)
		{
			return length;
		}
		return count - (lenNum * 2);
	}

	return count;
}

void TextWindow::Execute()noexcept
{
	if (timer < interval)
		timer += Time::deltaTime;
}

std::string TextWindow::utf8_substr(const std::string& str, unsigned int start, unsigned int leng)noexcept
{
	if (leng == 0) { return ""; }
	unsigned int c = 0, itr = 0, strLength = 0, wordCount = 0,
		min = static_cast<unsigned int>(std::string::npos),
		max = static_cast<unsigned int>(std::string::npos);
	for (strLength = static_cast<unsigned int>(str.length())
		; itr < strLength; itr++, wordCount++)
	{
		if (wordCount == start) { min = itr; }
		if (wordCount <= start + leng || leng == std::string::npos) { max = itr; }

		c = (unsigned char)str[itr];
		if (c >= 0 && c <= 127)itr += 0;
		else if ((c & 0xE0) == 0xC0)itr += 1;
		else if ((c & 0xF0) == 0xE0)itr += 2;
		else if ((c & 0xF8) == 0xF0)itr += 3;
		else if ((c & 0xFC) == 0xF8)itr += 4;
		else if ((c & 0xFE) == 0xFC)itr += 5;
		else return "";
	}
	if (wordCount <= start + leng || leng == std::string::npos) { max = itr; }
	if (min == std::string::npos || max == std::string::npos) { return ""; }
	if (start > wordCount) { return ""; }
	if (strLength >= count)count++;

	return str.substr(min, max);
}

void TextWindow::Reset() noexcept
{
	timer = 0.f;
	textItr = 0;
	count = 1;
}

bool TextWindow::WriteText(std::string name,
	std::vector<std::string> text, MODE mode)noexcept
{	
	ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	Execute();
	ImGui::Text(name.c_str());
	switch (mode)
	{
	case TextWindow::MODE::LbL:
		for (unsigned int i = 0; i <= count / length; i++)
		{
			int lenNum = i * length;
			ImGui::Text(
				(utf8_substr(text[textItr], lenNum, TextLineMax(lenNum))).c_str(), 100);
		}
		break;
	case TextWindow::MODE::Flash:
		if (static_cast<unsigned int>(text[textItr].length()) > length)
		{
			unsigned int loop = static_cast<unsigned int>(text[textItr].length()) / length;
			for (unsigned int i = 0; i < loop; i++)
			{
				int lenNum = i * length;
				ImGui::Text(
					(utf8_substr(text[textItr], lenNum, length)).c_str(), 100);
			}
		}
		else
			ImGui::Text(text[textItr].c_str());
		break;
	default:
		break;
	}

	if (timer > interval)
	{
		if (InputSystem::GetKey(GLFW_KEY_ENTER))
		{
			if (mode == MODE::LbL && text[textItr].length() > count)
			{
				count = static_cast<unsigned int>(text[textItr].length());
			}
			else if (text.size() != (textItr + 1))
			{
				textItr++; count = 1;
			}
			else
			{
				ImGui::End();
				return true;
			}
			timer = 0.0f;
		}
	}

	ImGui::End();
	return false;
}
