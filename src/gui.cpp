#include "gui.h"

bool Gui::Initialize()
{
    m_base_window_size = ImVec2(400, 100);
    m_window_size = m_base_window_size;
    m_window_pos = ImVec2(100, 100);
    m_clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

    m_basic_window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        std::cerr << "Failed to init SDL2." << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GetCurrentDisplayMode(0, &m_display_mode);
    m_window_ptr = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_base_window_size.x, m_base_window_size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    m_gl_context = SDL_GL_CreateContext(m_window_ptr);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = ImGui::GetIO();

    ImGui_ImplSDL2_InitForOpenGL(m_window_ptr, m_gl_context);
    ImGui_ImplOpenGL2_Init();

    ImGui::StyleColorsDark();


    m_button_states.resize(100);
}

void Gui::Shutdown()
{
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_window_ptr);
    SDL_Quit();
}

void Gui::Process()
{
    SDL_SetWindowPosition(m_window_ptr, m_window_pos.x, m_window_pos.y);
    UpdateWindowSize();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            m_finsihed = true;
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window_ptr);
    ImGui::NewFrame();

    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(m_base_window_size);
        ImGui::Begin("PoEPricey", 0, m_basic_window_flags);

        const char* currency_item_list[] = {"Alteration orb", "Fusing orb", "Alchemy orb", "Chaos orb", "Gemcutter's orb",
                                            "Exalted orb", "Chromatic orb", "Jeweller orb", "Chance orb", "Chisel orb",
                                            "Scouring orb", "Blessed orb", "Regret orb", "Regal orb", "Divine orb",
                                            "Vaal orb", "Scroll of wisdom", "Portal scroll", "Armourer's Scrap",
                                            "Blacksmith's whetstone", "Glassblowers bauble", "Transmutation orb",
                                            "Augumentation orb", "Mirror of Kalandra", "Eternal orb",
                                            "Perandus coin", "Silver coin"};

        ImGui::PushItemWidth(100);
        ImGui::Combo("Buy", &m_selected_currency_to_buy, currency_item_list, 27);
        ImGui::Combo("Sell", &m_selected_currency_to_sell, currency_item_list, 27);

        if(ImGui::Button("Get price"))
            SwitchButtonState(GET_PRICE_BUTTON, true);

        ImGui::SameLine();
        if(ImGui::Button("Close"))
            m_finsihed = true;

        if(ImGui::IsMouseDragging() && ImGui::IsRootWindowFocused())
        {
            ImVec2 mouse_drag_delta = ImGui::GetMouseDragDelta(0, 0.0f);
            m_window_pos = ImVec2(m_window_pos.x + mouse_drag_delta.x, m_window_pos.y + mouse_drag_delta.y);
        }

        ImGui::End();

        if(!m_price_lists.empty())
        {
            m_window_size = ImVec2(m_base_window_size.x, m_base_window_size.y + 300 * m_price_lists.size());
            for(int i = 0; i < m_price_lists.size() && !m_price_lists.empty(); i++)
            {
                std::vector<Item> item_vector(m_price_lists[i]);
                std::string price_window_name("");

                price_window_name = price_window_name +
                                    "Price list (" +
                                    GetCurrencyName(item_vector[0].s_item_type, true) +
                                    ";" +
                                    GetCurrencyName(item_vector[0].s_price_item_type, true) +
                                    ")";

                ImGui::SetNextWindowPos(ImVec2(0, 100 + 300 * i));
                ImGui::SetNextWindowSize(ImVec2(400, 300));
                ImGui::Begin(price_window_name.c_str(), 0, m_basic_window_flags);

                for(int j = 0; j < item_vector.size(); j++)
                {
                    std::string item_text_buf;
                    item_text_buf = item_text_buf + "Selling " + GetCurrencyName(item_vector[j].s_item_type, true) +
                                                    "(" + std::to_string((int)item_vector[j].s_sell_price) + ") " +
                                                    "for " + GetCurrencyName(item_vector[j].s_price_item_type, true) +
                                                    "(" + std::to_string((int)item_vector[j].s_buy_price) + ") " +
                                                    "Name: " + item_vector[j].s_seller_acc_name;

                    ImGui::PushItemWidth(350);
                    ImGui::Text(item_text_buf.c_str());

                    ImGui::Button("Buy");
                    ImGui::SameLine(); ImGui::Button("Message");
                    ImGui::SameLine(); ImGui::Button("Add to ignore list");
                }

                if(ImGui::Button("Close"))
                {
                    m_price_lists.erase(m_price_lists.begin() + i);
                    m_window_size.y -= 300;
                }

                ImGui::End();
            }
        }
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)m_io.DisplaySize.x, (int)m_io.DisplaySize.y);
    glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window_ptr);
}
