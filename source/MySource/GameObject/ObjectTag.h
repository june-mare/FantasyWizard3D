#pragma once
#include <string>
#include <vector>
#include "System/FileSystem.h"
#include "DataStruct/useImGui.h"

namespace TAG
{ 
    void Initialize(const char* path)noexcept;
    
    std::string CheckforResistation(std::string tag)noexcept;
   
    void ReadFile(const char* path)noexcept;

    void SaveFile(const char* path)noexcept;
    
    std::vector<std::string> GetTags()noexcept;
  
    void DebugTextOut(const char* path);
    
    void Finalize()noexcept;
};