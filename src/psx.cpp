#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl3w.h>
#include <fstream>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "common.h"
#include "log.h"
#include "cpu.h"
#include "spu.h"
#include "bios.h"
#include "ram.h"
#include "interconnect.h"

#include "psx.h"

#define FPS                     30
#define GLSL_VERSION            "#version 130"

#define DEBUGGER_WIDTH          800
#define DEBUGGER_HEIGHT         600


PSX::~PSX()
{
    delete cpu;

    cpu = nullptr;

    SDL_Quit();
}


bool PSX::init(std::string bios_path, std::string rom_path)
{
    this->bios_path = bios_path;
    this->rom_path = rom_path;

    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error("Unable to initialize SDL\n");
        return false;
    }

    cpu = new CPU();
    spu = new SPU();
    bios = new BIOS();
    ram = new RAM();
    inter = new Interconnect();

    running = true;
    running &= cpu->init();
    running &= spu->init();
    running &= bios->init(bios_path);
    running &= ram->init();
    running &= inter->init(spu, bios, ram);

    running &= initGUI();

    cpu->set_inter(inter);

    return running;
}


bool PSX::initGUI()
{
    // GL 3.0 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    sdl_window = SDL_CreateWindow(
        "PSX",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEBUGGER_WIDTH,
        DEBUGGER_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if(sdl_window == NULL) {
        error("Unable to create a display window\n");
        return false;
    }

    gl_context = SDL_GL_CreateContext(sdl_window);
    if (!gl_context) {
        error("Unable to init GL/SDL context!\n");
        return false;
    }

    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (gl3wInit() != 0) {
        error("Failed to initialize OpenGL loader!\n");
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontDefault();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, gl_context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    show_cpu = false;
    show_memory = false;
    show_execution = false;
    show_breakpoints = false;
    show_gpu = false;

    last_refresh = 0;

    return true;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int PSX::run()
{
    while (running) {
        draw();
        handle_events();

        cpu->run_next();
    }

    return EXIT_SUCCESS;
}


void PSX::draw()
{
    // Display
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + (1000 / FPS)) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        ToggleButton("CPU", &show_cpu);
        ToggleButton("Memory", &show_memory);
        ToggleButton("Execution", &show_execution);
        ToggleButton("Breakpoints", &show_breakpoints);
        ToggleButton("GPU", &show_gpu);

        if (ImGui::Button("<")) {
            // Save slot previous
        }
        ImGui::Text("Save slot #%d", 0);
        if (ImGui::Button(">")) {
            // Save slot next
        }
    }
    ImGui::EndMainMenuBar();

    if (show_cpu) cpu->display_registers(&show_cpu);
    if (show_memory) display_memory();
    if (show_execution) display_execution();
    if (show_breakpoints) display_breakpoints();
    if (show_gpu) display_gpu();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);

    last_refresh = current_ticks;
}


/**
 * @brief      Dispatch process time to each PSX component
 */
void PSX::process()
{
}


void PSX::handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        // Game inputs
        /*if (event.window.windowID == gpu->get_window_id()) {
            input->handle(&event);
        }*/

        switch(event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_WINDOWEVENT:
            switch(event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                /*Uint32 window_id = event.window.windowID;
                if (window_id == gpu->get_window_id()) {
                    running = false;
                }*/
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
            case SDLK_F1:   // Show debugger
                // TODO
                break;
            case SDLK_F2:   // Activate breakpoints
                // TODO
                break;
            case SDLK_F3:   // Save state
                // TODO
                break;
            case SDLK_F4:   // Next slot
                // TODO
                break;
            case SDLK_F5:   // Load state
                // TODO
                break;
            case SDLK_F9:   // Resume/Break process
                // TODO
                break;
            case SDLK_F10:  // Load another rom
                // TODO
                break;
            }
            break;
        }
    }
}


void PSX::reset()
{
    cpu->reset();
}


/**
 * @brief      Loads the given ROM
 * @param[in]  filepath  The filepath
 */
void PSX::load_rom(std::string filepath)
{
    // TODO
}


void PSX::display_memory()
{
    // TODO
}


void PSX::display_execution()
{
    // TODO
}


void PSX::display_breakpoints()
{
    // TODO
}


void PSX::display_gpu()
{
    // TODO
}

