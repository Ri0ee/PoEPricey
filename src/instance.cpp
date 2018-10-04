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

    m_base_url = "http://currency.poe.trade/";
    m_league_name = "Delve";

    curl_version_info_data* c_inf = curl_version_info(CURLVERSION_NOW);

    /// Because i need to inflate gzipped html page
    if(std::string(c_inf->libz_version) != "1.2.8" || std::string(c_inf->version) != "7.62.0-DEV")
    {
        std::cout << "Bad curl.dll version." << std::endl;
        return false;
    }

    m_curl_ptr = curl_easy_init();

    return true;
}

bool Instance::Run()
{
    m_gui_ptr = new Gui();
    m_gui_ptr->Initialize();
    while(!m_gui_ptr->IsFinished())
    {
        m_gui_ptr->Process();

        if(m_gui_ptr->GetButtonState(GET_PRICE_BUTTON))
        {
            auto get_itemprice_responce = GetItemPrice(true, EXALTED_ORB, CHAOS_ORB, 0);
            if(!get_itemprice_responce.second)
            {
                std::cerr << "Failed to retrieve item price." << std::endl;
                return false;
            }

            std::vector<Item> ex_chaos_prices = get_itemprice_responce.first;

            for(auto it = ex_chaos_prices.begin(); it != ex_chaos_prices.end(); it++)
            {
                std::cout << it->s_buy_price << std::endl;
            }

            m_gui_ptr->SwitchButtonState(GET_PRICE_BUTTON, false);
        }
    }

    m_gui_ptr->Shutdown();

    return true;
}

void Instance::Shutdown()
{
    std::cout << "Shutting down..." << std::endl;
    curl_easy_cleanup(m_curl_ptr);
}

std::pair<std::string, bool> Instance::GetPage(const std::string& page_address_)
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
            std::cerr << curl_easy_strerror(result) << std::endl;
            return std::pair<std::string, bool>(std::string(""), false);
        }

        return std::pair<std::string, bool>(buffer, true);
    }
    else
    {
        std::cerr << "Failed to get page. Curl is not initalized" << std::endl;
        return std::pair<std::string, bool>(std::string(""), false);
    }
}

std::pair<std::vector<Item>, bool> Instance::GetItemPrice(bool online_only_, int item_type_, int price_item_type_, int min_stock_)
{
    TradeURL trade_url(m_base_url, m_league_name, online_only_, item_type_, price_item_type_, min_stock_);
    trade_url.Compose();
    auto page_url = trade_url.Get();
    auto get_page_response = GetPage(page_url);
    std::vector<Item> temp_item_vec;

    if(!get_page_response.second)
    {
        std::cerr << "Failed to get item price." << std::endl;
        return std::pair<std::vector<Item>, bool>(std::vector<Item>(), false);
    }

    std::string page_code = get_page_response.first;
    std::string block_start_code = "<div class=\"displayoffer \"";
    std::string block_end_code = ">";

    std::string data_username_code = "data-username";
    std::string data_ign_code = "data-ign";
    std::string data_sellvalue_code = "data-sellvalue";
    std::string data_buyvalue_code = "data-buyvalue";
    std::string data_stock_code = "data-stock";

    auto occurrence_it = std::search(page_code.begin(), page_code.end(), block_start_code.begin(), block_start_code.end());
    if(occurrence_it == page_code.end())
    {
        std::cerr << "Failed to find first div occurrence." << std::endl;
        return std::pair<std::vector<Item>, bool>(std::vector<Item>(), false);
    }
    page_code.erase(page_code.begin(), occurrence_it);

    while(true)
    {
        auto block_start_occurrence_it = std::search(page_code.begin(), page_code.end(), block_start_code.begin(), block_start_code.end());
        auto block_end_occurrence_it = std::search(block_start_occurrence_it, page_code.end(), block_end_code.begin(), block_end_code.end());

        if(block_end_occurrence_it == page_code.end() || block_start_occurrence_it == page_code.end())
            break;
        else
        {
            bool is_stock_present = true;

            std::string data_block(block_start_occurrence_it, block_end_occurrence_it + 2);
            page_code.erase(page_code.begin(), block_end_occurrence_it);

            auto acc_name_it  = std::search(data_block.begin(), data_block.end(), data_username_code.begin(), data_username_code.end()) + data_username_code.size() + 1;
            auto ign_it       = std::search(data_block.begin(), data_block.end(), data_ign_code.begin(), data_ign_code.end()) + data_ign_code.size() + 1;
            auto sellvalue_it = std::search(data_block.begin(), data_block.end(), data_sellvalue_code.begin(), data_sellvalue_code.end()) + data_sellvalue_code.size() + 1;
            auto buyvalue_it  = std::search(data_block.begin(), data_block.end(), data_buyvalue_code.begin(), data_buyvalue_code.end()) + data_buyvalue_code.size() + 1;
            auto stock_it     = std::search(data_block.begin(), data_block.end(), data_stock_code.begin(), data_stock_code.end()) + data_stock_code.size() + 1;
            if(stock_it - data_stock_code.size() - 1 == data_block.end())
                is_stock_present = false;

            int acc_name_dist = std::distance(data_block.begin(), acc_name_it);
            int ign_dist = std::distance(data_block.begin(), ign_it);
            int sellvalue_dist = std::distance(data_block.begin(), sellvalue_it);
            int buyvalue_dist = std::distance(data_block.begin(), buyvalue_it);
            int stock_dist = std::distance(data_block.begin(), stock_it);

            std::string acc_name = data_block.substr(acc_name_dist + 1, data_block.find("\"", acc_name_dist + 1) - acc_name_dist - 1);
            std::string char_name = data_block.substr(ign_dist + 1, data_block.find("\"", ign_dist + 1) - ign_dist - 1);
            std::string sellvalue = data_block.substr(sellvalue_dist + 1, data_block.find("\"", sellvalue_dist + 1) - sellvalue_dist - 1);
            std::string buyvalue = data_block.substr(buyvalue_dist + 1, data_block.find("\"", buyvalue_dist + 1) - buyvalue_dist - 1);
            std::string stock = "0";
            if(is_stock_present)
                stock = data_block.substr(stock_dist + 1, data_block.find("\"", stock_dist + 1) - stock_dist - 1);

            Item temp_item {item_type_, price_item_type_, std::stof(sellvalue), std::stof(buyvalue), std::stoi(stock), acc_name, char_name};
            temp_item_vec.push_back(temp_item);
        }
    }

    return std::pair<std::vector<Item>, bool>(temp_item_vec, true);
}
