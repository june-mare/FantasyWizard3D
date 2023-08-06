#pragma once
#ifndef TILEMAP_H_INCLUDE
#define TILEMAP_H_INCLUDE
#include <vector>

/*タイルの配置データ*/
class TileMap
{
public:
	size_t sizex = 0;
	size_t sizey = 0;
	std::vector<int> data;
};

#include "glad/glad.h"
#include <string>
#include <fstream>
#include <filesystem>
class TileMapScroll;

namespace LoadTileMap
{
TileMap LoadTileMap(const char* filename,TileMapScroll scrolldata)noexcept;
};

#endif
