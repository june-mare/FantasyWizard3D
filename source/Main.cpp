#include <Windows.h>
#include "Engine/Engine.h"

INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,			
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR IpCmdLine,
	_In_ INT nShowCmd){
	Engine& engine = Engine::Instance();
	return engine.Run();
}