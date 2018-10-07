#include "gui.h"

bool Gui::Initialize()
{
    m_base_window_size = ImVec2(400, 100);
    m_window_size = m_base_window_size;
    m_window_pos = ImVec2(100, 100);
    m_clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);
    m_max_price_results = 10;

    m_show_options_window = false;
    m_show_ignore_list_window = false;

    m_selected_currency_to_buy = 0;
    m_selected_currency_to_sell = 0;
    m_selected_league = 0;

    m_basic_window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_DECORATED, GL_FALSE);

    m_window_ptr = glfwCreateWindow(m_window_size.x, m_window_size.y, "", NULL, NULL);
    if (m_window_ptr == NULL)
        return 1;

    glfwMakeContextCurrent(m_window_ptr);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(m_window_ptr, true);
    ImGui_ImplOpenGL2_Init();

    ImGui::StyleColorsDark();

    m_currency_items = {std::string("Alteration orb"), std::string("Fusing orb"), std::string("Alchemy orb"),
                        std::string("Chaos orb"), std::string("Gemcutter's orb"), std::string("Exalted orb"),
                        std::string("Chromatic orb"), std::string("Jeweller orb"), std::string("Chance orb"),
                        std::string("Chisel orb"), std::string("Scouring orb"), std::string("Blessed orb"),
                        std::string("Regret orb"), std::string("Regal orb"), std::string("Divine orb"),
                        std::string("Vaal orb"), std::string("Scroll of wisdom"), std::string("Portal scroll"),
                        std::string("Armourer's Scrap"), std::string("Blacksmith's whetstone"), std::string("Glassblowers bauble"),
                        std::string("Transmutation orb"), std::string("Augumentation orb"), std::string("Mirror of Kalandra"),
                        std::string("Eternal orb"), std::string("Perandus coin"), std::string("Silver coin")};

    m_league_names = {std::string("Delve SC"), std::string("Delve HC"), std::string("Standard SC"), std::string("Standard HC")};

    m_button_states.resize(100);
}

void Gui::Shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window_ptr);
    glfwTerminate();
}

void Gui::Process()
{
    glfwSetWindowPos(m_window_ptr, m_window_pos.x, m_window_pos.y);
    UpdateWindowSize();

    glfwPollEvents();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(m_base_window_size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("PoEPricey", 0, m_basic_window_flags);

        ImGui::PushItemWidth(100);

        if(ImGui::BeginCombo("Buy", m_currency_items[m_selected_currency_to_buy].c_str()))
        {
            for(int n = 0; n < m_currency_items.size(); n++)
            {
                bool is_selected = (m_currency_items[m_selected_currency_to_buy] == m_currency_items[n]);
                if(ImGui::Selectable(m_currency_items[n].c_str(), is_selected))
                    m_selected_currency_to_buy = n;
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        ImGui::InputInt("Maximum results", &m_max_price_results);
        if(m_max_price_results < 0)
            m_max_price_results = 0;

        if(ImGui::BeginCombo("Sell", m_currency_items[m_selected_currency_to_sell].c_str()))
        {
            for(int n = 0; n < m_currency_items.size(); n++)
            {
                bool is_selected = (m_currency_items[m_selected_currency_to_sell] == m_currency_items[n]);
                if(ImGui::Selectable(m_currency_items[n].c_str(), is_selected))
                    m_selected_currency_to_sell = n;
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        if(ImGui::BeginCombo("League", m_league_names[m_selected_league].c_str()))
        {
            for(int n = 0; n < m_league_names.size(); n++)
            {
                bool is_selected = (m_league_names[m_selected_league] == m_league_names[n]);
                if(ImGui::Selectable(m_league_names[n].c_str(), is_selected))
                    m_selected_league = n;
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if(ImGui::Button("Get price"))
            SwitchButtonState(GET_PRICE_BUTTON, true);

        ImGui::SameLine();
        if(ImGui::Button("Options"))
            m_show_options_window != m_show_options_window;

        ImGui::SameLine();
        if(ImGui::Button("Show ignore list"))
            m_show_ignore_list_window != m_show_ignore_list_window;

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
                                    ")###" + std::to_string(i);

                ImGui::SetNextWindowPos(ImVec2(0, 100 + 300 * i));
                ImGui::SetNextWindowSize(ImVec2(400, 300));
                ImGui::Begin(price_window_name.c_str(), 0, m_basic_window_flags);

                for(int j = 0; j < item_vector.size() && j < m_max_price_results; j++)
                {
                    std::string item_text_buf;
                    item_text_buf = item_text_buf +
                                    "Selling " + GetCurrencyName(item_vector[j].s_item_type, true) +
                                    "(" + std::to_string((int)item_vector[j].s_sell_price) + ") " +
                                    "for " + GetCurrencyName(item_vector[j].s_price_item_type, true) +
                                    "(" + std::to_string((int)item_vector[j].s_buy_price) + ") " +
                                    "Name: " + item_vector[j].s_seller_acc_name;

                    ImGui::PushItemWidth(350);

                    ImGui::PushID(j);
                    ImGui::Text(item_text_buf.c_str());

                    if(ImGui::Button("Buy"))
                    {
                        std::string buy_message_buf("");
                        buy_message_buf =   buy_message_buf +
                                            "@" + item_vector[j].s_seller_acc_name +
                                            " WTB your " + std::to_string((int)item_vector[j].s_sell_price) +
                                            " " + GetCurrencyName(item_vector[j].s_item_type, false) +
                                            " listed for " + std::to_string((int)item_vector[j].s_buy_price) +
                                            " " + GetCurrencyName(item_vector[j].s_price_item_type, false);

                        ImGui::SetClipboardText(buy_message_buf.c_str());
                    }

                    ImGui::SameLine();
                    if(ImGui::Button("Message"))
                    {
                        std::string pm_message_buf("");
                        pm_message_buf =    pm_message_buf +
                                            "@" + item_vector[j].s_seller_acc_name + " ";

                        ImGui::SetClipboardText(pm_message_buf.c_str());
                    }

                    ImGui::SameLine();
                    if(ImGui::Button("Add to ignore list"))
                    {
                        m_account_ignore_list.push_back(item_vector[j].s_seller_acc_name);
                    }

                    ImGui::PopID();
                }

                if(ImGui::Button("Close"))
                {
                    m_price_lists.erase(m_price_lists.begin() + i);
                    m_window_size.y -= 300;
                }

                ImGui::End();
            }
        }
        ImGui::PopStyleVar(1);
    }

    // Rendering
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(m_window_ptr, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(m_window_ptr);
    glfwSwapBuffers(m_window_ptr);
}
