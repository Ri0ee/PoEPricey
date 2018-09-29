#include "instance.h"
#include "data_header.h"

static size_t WriteCallback(char* contents_, size_t size_, size_t nmemb_, void* userdata_)
{
    ((std::string*)userdata_)->append((char*)contents_, size_ * nmemb_);
    return size_ * nmemb_;
}

bool Instance::Initialize(char* path_to_exefile_)
{
    m_path_to_exefile = path_to_exefile_;
    m_path_to_dir = m_path_to_exefile.substr(0, m_path_to_exefile.find_last_of("/\\"));

    m_base_url = "http://currency.poe.trade";
    m_league_name = "Delve";

    curl_version_info_data* c_inf = curl_version_info(CURLVERSION_NOW);

    /// Because i need to inflate gzipped html page
    if(strcmp(c_inf->libz_version, "1.2.8") != 0 || strcmp(c_inf->version, "7.62.0-DEV") != 0)
    {
        std::cout << "Bad curl.dll version." << std::endl;
        return false;
    }

    m_curl_ptr = curl_easy_init();

    return true;
}

bool Instance::Run()
{
    std::string PageCode = GetPage(m_base_url);
    //std::cout << PageCode << std::endl;

    return true;
}

void Instance::Shutdown()
{
    std::cout << "Shutting down..." << std::endl;
    curl_easy_cleanup(m_curl_ptr);
}

std::string Instance::GetPage(const std::string& page_address_)
{
    if(m_curl_ptr)
    {
        std::string buffer;
        CURLcode result;

        curl_easy_setopt(m_curl_ptr, CURLOPT_URL, page_address_.c_str());
        curl_easy_setopt(m_curl_ptr, CURLOPT_ACCEPT_ENCODING, "gzip");

        curl_easy_setopt(m_curl_ptr, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl_ptr, CURLOPT_WRITEDATA, &buffer);

        result = curl_easy_perform(m_curl_ptr);
        if(result != CURLE_OK)
        {
            std::cerr << "curl_easy_perform error: " << curl_easy_strerror(result) << std::endl;
            return std::string("");
        }

        return buffer;
    }
    else
    {
        std::cout << "Failed to get page. Curl is not initalized" << std::endl;
        return std::string("");
    }
}
