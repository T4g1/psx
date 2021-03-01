#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "log.h"
#include "cpu.h"
#include "bios.h"
#include "ram.h"
#include "interconnect.h"

#include "psx.h"


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
    bios = new BIOS();
    ram = new RAM();
    inter = new Interconnect();

    running = true;
    running &= cpu->init();
    running &= bios->init(bios_path);
    running &= ram->init();
    running &= inter->init(bios, ram);

    if (running) {
        cpu->set_inter(inter);
    }

    return running;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int PSX::run()
{
    while (running) {
        cpu->run_next();
    }

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
