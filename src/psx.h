#ifndef PSX_H
#define PSX_H

#include <iostream>
#include <string>

class CPU;
class SPU;
class BIOS;
class RAM;
class Interconnect;


/**
 * @brief      PSX Emulator
 */
class PSX {
    CPU *cpu;
    SPU *spu;
    BIOS *bios;
    RAM *ram;
    Interconnect *inter;

    bool running;
    bool no_boot;

    std::string bios_path;
    std::string rom_path;

    SDL_Window *sdl_window;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    // Which window to display
    bool show_cpu;
    bool show_memory;
    bool show_execution;
    bool show_breakpoints;
    bool show_gpu;

    Uint32 last_refresh;

    void display_memory();
    void display_execution();
    void display_breakpoints();
    void display_gpu();

public:
    size_t save_slot;

    ~PSX();

    bool init(std::string bios_path, std::string rom_path);
    bool initGUI();
    int run();
    void draw();
    void process();
    void handle_events();
    void reset();

    void load_rom(std::string filepath);
};

#endif /* PSX_H */
