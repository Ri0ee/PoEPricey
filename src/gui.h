#pragma once

#include <iostream>
#include <vector>

#include "include/imgui/imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"

#define SDL_MAIN_HANDLED
#include <include/SDL2/SDL.h>
#include <include/SDL2/SDL_opengl.h>

class Gui
{
public:
    Gui() {}
    ~Gui() {}

    bool Initialize();
    void Shutdown();
    void Process();

    bool IsFinished() { return m_finsihed; }

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

private:
    SDL_Window* m_window_ptr = nullptr;
    SDL_GLContext m_gl_context;
    SDL_DisplayMode m_display_mode;
    ImGuiIO m_io;

    bool m_finsihed;
    ImVec4 m_clear_color;
    ImVec2 m_window_size;
    ImVec2 m_window_pos;

    std::vector<bool> m_button_states;
};

#define GET_PRICE_BUTTON 1
