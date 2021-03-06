#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "include/imgui/imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glfw.h"
#include "struct.h"
#include "util.h"

#include "include/GLFW/glfw3.h"

class Gui
{
public:
    Gui() {}
    ~Gui() {}

    bool Initialize();
    void Shutdown();
    void Process();

    void UpdateWindowSize()
    {
        glfwSetWindowSize(m_window_ptr, m_window_size.x, m_window_size.y);
    }

    bool IsFinished() { return m_finsihed; }
    int GetSelectedCurrencyToBuy() { return m_selected_currency_to_buy + 1; }
    int GetSelectedCurrencyToSell() { return m_selected_currency_to_sell + 1; }

    bool GetButtonState(int button_id_)
    {
        if(button_id_ > m_button_states.size())
            return false;
        else
            return m_button_states[button_id_];
    }

    void SwitchButtonState(int button_id_, bool new_state_)
    {
        if(button_id_ > m_button_states.size())
            return;
        else
            m_button_states[button_id_] = new_state_;
    }

    void PushPriceList(std::vector<Item> m_price_list_)
    {
        m_price_lists.push_back(m_price_list_);
    }

private:
    GLFWwindow* m_window_ptr;


    ImGuiIO m_io;

    bool m_finsihed;
    ImVec4 m_clear_color;
    ImVec2 m_window_size;
    ImVec2 m_base_window_size;
    ImVec2 m_window_pos;
    ImGuiWindowFlags m_basic_window_flags;

    std::vector<std::string> m_league_names;
    std::vector<std::string> m_currency_items;
    std::vector<std::string> m_account_ignore_list;

    std::vector<bool> m_button_states;

    std::vector<std::vector<Item>> m_price_lists;
    int m_selected_currency_to_buy;
    int m_selected_currency_to_sell;
    int m_selected_league;

    bool m_show_options_window;
    bool m_show_ignore_list_window;

    int m_max_price_results;
};

class Gui_options
{
public:

private:

};

#define GET_PRICE_BUTTON 1
