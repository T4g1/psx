#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <cstdint>

class BIOS;
class RAM;


/**
 * @brief      Handles virtual memory mapping
 * Dispatch read/write request to correct modules and/or memory
 */
class Interconnect {
    BIOS *bios;
    RAM *ram;

public:
    ~Interconnect();

    bool init(BIOS *bios, RAM *ram);
    void store32(uint32_t address, uint32_t value);
    uint32_t load32(uint32_t address);
};

#endif /* INTERCONNECT_H */
