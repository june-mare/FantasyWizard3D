#pragma once
#include <string>
#include <vector>

class TextWindow
{
	float 	interval = 0.15f;
	float 	timer = 0.f;
	int     textItr = 0;
	unsigned int 	length = 50;
	unsigned int		count = 1;

	void Execute()noexcept;
	std::string utf8_substr(const std::string& str, unsigned int start, unsigned int leng)noexcept;
	unsigned int TextLineMax(int lenNum)noexcept;
public:
	enum class MODE
	{
		LbL,       //latter by latter
		Flash,     //All latter Flash
	};
	MODE mode = MODE::LbL;
	void Reset()noexcept;
	bool WriteText(std::string name,
		std::vector<std::string> text, MODE mode = MODE::LbL)noexcept;
};