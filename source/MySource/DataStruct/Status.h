#pragma once
/*
* @brief ゲームオブジェクトに使用するステータスの管理や読み書き
* @auther Junya Sasou
* @date  2023/03/22
*/
#include <unordered_map>
#include <stdarg.h>
#include <string>

namespace STATUS
{
	/*先行宣言*/
	struct Status;

	/*指定したファイルからステータスを読み込む
	* @param const char* テキストファイルのパス
	* @param std::string 取得したいオブジェクトの名前
	* @return 取得できたステータス*/
	Status ReadFileToStatus(const char* path, const char* name)noexcept;

	/*指定したステータスをファイルへ保存する
	* @param const char* テキストファイルのパス
	* @param std::string 取得したいオブジェクトの名前*/
	void SaveStatusToFile(const char* path, Status status)noexcept;

	/*
	* @brief ゲームに使う状態異常のenum
	*/
	enum class Condision
	{
		normal,
		poizon,
		freeze,
	};

	/**
	* @brief ゲームオブジェクトで使うパラメータ保存用構造体
	*/
	struct Status
	{
	private:
		std::string name;
		std::unordered_map<std::string, float>
			package;
		Condision condision = Condision::normal;
	public:
		/*指定したステータスの値のゲッター*/
		inline float GetStatusValue(const char* statusname)const noexcept
		{
			if (package.find(statusname) != package.end())
				return package.at(statusname);
			return 0.0f;
		}
		/*ステータスのマップのゲッター*/
		inline std::unordered_map<std::string, float>&
			GetStatuses()noexcept
		{
			return package;
		}
		/*状態異常のゲッター*/
		inline Condision GetCondision()const noexcept
		{
			return condision;
		}
		/*名前のゲッター*/
		inline std::string GetName()const noexcept
		{
			return name;
		}
		/*名前のセッター*/
		inline void SetName(std::string name)noexcept
		{
			this->name = name;
		}
		/*既存のパラメータのみ追加*/
		inline void SetStatusValue(const char* statusname, float value)noexcept
		{
			if (package.find(statusname) != package.end())
				package[statusname] = value;
		}
		/*既存や新規のパラメータ追加*/
		inline void PushStatusValue(const char* statusname, float value)noexcept
		{
			package[statusname] = value;
		}
		/*状態異常のセッター*/
		inline void SetCondision(Condision _condision)noexcept
		{
			condision = _condision;
		}
		/*一括既存と新規のっパラメータ追加
		* @param int 第一引数を除いた引数の数
		* @param ... 可変長引数　std::string, float ...の順で記載する*/
		void SetStatusValues(int num, ...)noexcept
		{
			va_list args;
			va_start(args, num);
			std::string name = {};float param = 0.0f;
			for (int i = 0; i < num; i++)
			{
				if (i % 2 == 0)
					name = va_arg(args, char*);
				else
				{
					param = static_cast<float>(va_arg(args, double));
					PushStatusValue(name.c_str(), param);
				}
			}
			va_end(args);
		}

		Status operator=(const Status& _status)
		{
			name = _status.name;
			package.clear();
			package.insert(_status.package.begin(), _status.package.end());
			condision = _status.condision;
			return _status;
		}

		float operator[](const char* name) {
			return GetStatusValue(name);
		}
		

	};

	/*
	* @brief ステータスを読み込んですぐのデータをリストとして所持する
	*/
	class StatusManager
	{
	private:
		StatusManager() {};
		~StatusManager() {
			SaveAllStatusToFile("resource/txt/Input.txt");
		}
		std::vector<Status> statuses = {};
	public:
		static StatusManager& GetInstance()
		{
			static StatusManager instance;
			return instance;
		}
		
		/*
		* @brief 元のデータから取得する（無い場合読み込む）
		* @param const char* ファイルまでのパス
		* @param string 取得したいオブジェクトの名前
		*/
		Status GetStatus(const char* path, const char* name)
		{
			for (auto itr : statuses)
			{
				if (itr.GetName().c_str() == name)
				{
					Status status;
					status = itr;
					return status;
				}
			}
			Status status = ReadFileToStatus(path, name);
			statuses.push_back(status);
			return status;
		}
		
		/*
		* 
		*/
		void SetStatus(Status status)
		{
			for (auto itr : statuses) {
				if (itr.GetName() == status.GetName())
					return;
			}
			statuses.push_back(status);			
		}

		/*
		* @brief 持っているすべての元データをセーブする
		* @param const char* ファイルまでのパス
		*/
		void SaveAllStatusToFile(const char* path)noexcept
		{
			for (auto itr : statuses)
			{
				SaveStatusToFile(path, itr);
			}
		}
		
		/*
		* @brief マネージャーにステータスを登録する
		* @param const char* ファイルまでのパス
		* @param const char* 取得するステータス名
		*/
		Status RegistStatusFromFile(const char* path, const char* name)noexcept{			
			Status status = ReadFileToStatus(path, name);
			statuses.push_back(status);
			return status;
		}

		/*
		* @brief 元データに追加及び変更する場合にのみ使う
		* @param Status 追加または変更するステータス型
		*/
		void SetOriginalStatus(Status status)noexcept
		{
			for (auto itr : statuses)
			{
				if (!std::strcmp(itr.GetName().c_str(), status.GetName().c_str()))
				{
					itr = status;
					return;
				}
			}
			Status copy = status;
			statuses.push_back(copy);
		}
	};

};
