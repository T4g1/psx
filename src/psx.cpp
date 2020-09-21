#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "log.h"

#include "psx.h"


PSX::~PSX()
{
    delete cpu;

    cpu = nullptr;

    SDL_Quit();
}


bool PSX::init(const char *bios_path, const char *rom_path)
{
    this->bios_path = "";
    if (bios_path != nullptr) {
        this->bios_path = bios_path;
    }

    this->rom_path = "";
    if (rom_path != nullptr) {
        this->rom_path = rom_path;
    }

    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error("Unable to initialize SDL\n");
        return false;
    }

    cpu = new CPU();

    running  = true;
    running &= cpu->init();

    return running;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int PSX::run()
{
    return EXIT_SUCCESS;
}


/**
 * @brief      Dispatch process time to each PSX component
 */
void PSX::process()
{
}


/**
 * @brief      Dispatch events to all subsystems
 */
void PSX::handle_events()
{
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
