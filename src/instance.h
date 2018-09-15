#pragma once

#include <string>
#include <iostream>

class Instance
{
public:
    Instance(char* path_to_exefile_);
    ~Instance() {}

    bool Initialize();
    bool Run();
    void Shutdown();

private:
    std::string m_path_to_exefile;
    std::string m_path_to_dir;
};
