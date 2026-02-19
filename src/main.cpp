// Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include <algorithm>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <cstdio>
#include <iostream>
#include <SDL3/SDL.h>
#include "Dijkstra.h"
#include "properties.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

static SDL_FRect grid{40, 40, UI::GRID_SIZE, UI::GRID_SIZE};
static float x_0 = grid.x;
static float y_0 = grid.y;
static float w  = grid.w;
static float h  = grid.h;
static float dx = w / static_cast<float>(UI::CELLS_X);
static float dy = h / static_cast<float>(UI::CELLS_Y);


inline void draw_rect(SDL_Renderer* renderer, const int x, const int y, const ImVec4& color) {
    SDL_SetRenderDrawColorFloat(renderer, color.x, color.y, color.z,color.w);
    const SDL_FRect rect(x_0 + x * dx + dx / 4.0, y_0 + y * dy + dy / 4.0, dx / 2, dy / 2);
    SDL_RenderFillRect(renderer, &rect);
}

inline void render_grid(SDL_Renderer* renderer) {
    grid = SDL_FRect{40, 40, UI::GRID_SIZE, UI::GRID_SIZE};
    x_0 = grid.x;
    y_0 = grid.y;
    w  = grid.w;
    h= grid.h;
    dx = w / static_cast<float>(UI::CELLS_X);
    dy = h / static_cast<float>(UI::CELLS_Y);

    SDL_RenderRect(renderer, &grid);

    for (int y = 1; y < UI::CELLS_Y; ++y) {
        const float ly = y_0 + y * dy;
        SDL_RenderLine(renderer, x_0, ly, x_0 + w, ly);
    }

    for (int x = 1; x < UI::CELLS_X; ++x) {
        const float lx = x_0 + x * dx;
        SDL_RenderLine(renderer, lx, y_0, lx, y_0 + h);
    }
}

#define DIJKSTRA 0
#define BMSSP 1

// Main code
int main(int, char**)
{
    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Create window with SDL_Renderer graphics context
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    bool done = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;

        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //show_steps_table();
        static int start_x = 0, start_y = 0;
        int i = 3;
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
        ImGui::InputInt("start x", &start_x);
        ImGui::InputInt("start y", &start_y);
        start_x = std::clamp(start_x, 0, UI::CELLS_X - 1);
        start_y = std::clamp(start_y, 0, UI::CELLS_Y - 1);
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        ImGui::Separator();

        static int e = 0;
        ImGui::RadioButton("Dijkstra", &e, 0); ImGui::SameLine();
        ImGui::SameLine();
        ImGui::RadioButton("BMSSP", &e, 1); ImGui::SameLine();

#define INDEX(x, y) (y * UI::CELLS_X + x)

        static std::vector<DijkstraFrame> dijkstra_frames;

        static int last_X = UI::CELLS_X;
        static int last_Y = UI::CELLS_Y;
        if (ImGui::Button("Execute")) {
            last_X = UI::CELLS_X;
            last_Y = UI::CELLS_Y;
            dijkstra_frames.clear();
            Graph graph(UI::CELLS_X, UI::CELLS_Y);
            std::cout << graph.size() << std::endl;
            if (e == DIJKSTRA) {
                Dijkstra dijkstra(graph, graph.get_vertex(INDEX(start_x, start_y)));
                auto dists = dijkstra.std_heap_run();
                dijkstra_frames = dijkstra.frames();
            }
        }

        static int frame = 0;
        static DijkstraFrame* current_frame = nullptr;

        if (!dijkstra_frames.empty()) {
            ImGui::SliderInt("Step", &frame, 0, static_cast<int>(dijkstra_frames.size())-1);
            current_frame = &dijkstra_frames[frame];
        }


        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        const auto&[bg_r, bg_g, bg_b, bg_w] = UI::wp_color;
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static int* dims[2] = {&UI::CELLS_X, &UI::CELLS_Y};
            static int counter = 0;
            ImGui::Begin("Hello World");
            ImGui::SliderFloat("Grid Width", &UI::GRID_SIZE, 400.0f, 1000.0f);
            ImGui::SliderInt2("Grid Width / Height", *dims, 8, 20);
            ImGui::ColorEdit3("Grid color", reinterpret_cast<float *>(&UI::grid_color));
            ImGui::ColorEdit3("Background color", reinterpret_cast<float *>(&UI::wp_color));


            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        // Background

        SDL_SetRenderDrawColorFloat(renderer, bg_r, bg_g, bg_b, bg_w);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColorFloat(renderer, UI::grid_color.x, UI::grid_color.y, UI::grid_color.z, UI::grid_color.w);

        // Grid
        render_grid(renderer);

        if (current_frame && UI::CELLS_X == last_X && UI::CELLS_Y == last_Y) {
            auto& f = *current_frame;

            for (int y = 0; y < UI::CELLS_Y; ++y) {
                for (int x = 0; x < UI::CELLS_X; ++x) {
                    if (f.finalized[INDEX(x,y)]) {
                        draw_rect(renderer, x, y, ImVec4(1,0,0,1));
                    }
                }
            }
        }

        draw_rect(renderer, start_x, start_y, ImVec4(0, 1, 0, 1));

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
