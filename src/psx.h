#ifndef PSX_H
#define PSX_H

#include <iostream>
#include <string>

class CPU;
class BIOS;


/**
 * @brief      PSX Emulator
 */
class PSX {
    CPU *cpu;
    BIOS *bios;

    bool running;
    bool no_boot;

    std::string bios_path;
    std::string rom_path;

public:
    size_t save_slot;

    ~PSX();

    bool init(std::string bios_path, std::string rom_path);
    int run();
    void process();
    void handle_events();
    void reset();

    void load_rom(std::string filepath);
};

#endif /* PSX_H */
