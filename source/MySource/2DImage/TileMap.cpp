#include "TileMap.h"
#include "TileMapScroll.h"

namespace LoadTileMap
{

	TileMap LoadTileMap(const char* filename, TileMapScroll scrolldata) noexcept
	{
		/*ファイルを開く*/
		std::ifstream file;
		file.open(filename);
		if (!file)
		{
			return {};
		}

		TileMap tileMap;
		tileMap.data.reserve(
			static_cast<int>(scrolldata.sizeX * scrolldata.sizeY));/*適当な容量を予約*/

		/*ファイルを読み込む*/
		while (!file.eof())
		{
			/*1行読み込む*/
			std::string line;
			std::getline(file, line);
			if (line.size() <= 0)
			{
				continue;/*データのない行を無視*/
			}
			line.push_back(',');/*終端を追加*/

			/*カンマ区切り文字列を数値に変換*/
			tileMap.sizex = 0;
			const char* start = line.data();
			const char* end = start + line.size();
			for (const char* p = start; p != end; ++p)
			{
				if (*p == ',') {
					const int n = atoi(start);
					tileMap.data.push_back(n);
					start = p + 1;/*次のカンマ区切りを指すように更新*/
					++tileMap.sizex;
				}
			}
			++tileMap.sizey;
		}

		return tileMap;
	}


}