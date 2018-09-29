#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "include/curl/curl.h"

struct MemoryStruct {
    char* memory;
    size_t size;
};

class Instance
{
public:
    Instance() {}
    ~Instance() {}

    bool Initialize(char* path_to_exefile_);
    bool Run();
    void Shutdown();

    std::string GetPage(const std::string& page_address_);

private:
    std::string m_path_to_exefile;
    std::string m_path_to_dir;

    std::string m_base_url;
    std::string m_league_name;

    CURL* m_curl_ptr = nullptr;
};
