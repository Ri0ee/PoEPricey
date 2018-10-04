#include "gui.h"

bool Gui::Initialize()
{
    m_window_size = ImVec2(400, 400);
    m_window_pos = ImVec2(100, 100);

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
    m_window_ptr = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_window_size.x, m_window_size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_TOOLTIP);
    m_gl_context = SDL_GL_CreateContext(m_window_ptr);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = ImGui::GetIO();

    ImGui_ImplSDL2_InitForOpenGL(m_window_ptr, m_gl_context);
    ImGui_ImplOpenGL2_Init();

    ImGui::StyleColorsDark();

    m_clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);
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
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            m_finsihed = true;
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window_ptr);
    ImGui::NewFrame();

    {
        ImGui::Begin("PoEPricey", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(m_window_size);

        if(ImGui::Button("Get price"))
            SwitchButtonState(GET_PRICE_BUTTON, true);

        if(ImGui::IsMouseDragging())
        {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            ImVec2 mouse_drag_delta = ImGui::GetMouseDragDelta();
            SDL_SetWindowPosition(m_window_ptr, );
        }

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)m_io.DisplaySize.x, (int)m_io.DisplaySize.y);
    glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window_ptr);
}
