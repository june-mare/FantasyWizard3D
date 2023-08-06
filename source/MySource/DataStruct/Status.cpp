#include "Status.h"
#include <fstream>
#include <sstream>
#include "System/FileSystem.h"

namespace STATUS
{
	/*指定したファイルからステータスを読み込む
	* @param const char* テキストファイルのパス
	* @param std::string 取得したいオブジェクトの名前
	* @return 取得できたステータス*/
	Status ReadFileToStatus(const char* path, const char* name) noexcept
	{
		Status status;
		status.SetName(name);
		FileSystem::ReadFile<std::string, float>(path, name, status.GetStatuses());
		return status;
	}

	/*指定したステータスをファイルへ保存する
	* @param const char* テキストファイルのパス
	* @param std::string 取得したいオブジェクトの名前*/
	void SaveStatusToFile(const char* path, Status status)noexcept
	{
		FileSystem::SaveFile<std::string, float>(path, status.GetName().c_str(),
			status.GetStatuses());
	}

}