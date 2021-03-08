#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <cstdint>

class SPU;
class BIOS;
class RAM;


/**
 * @brief      Handles virtual memory mapping
 * Dispatch read/write request to correct modules and/or memory
 */
class Interconnect {
    SPU *spu;
    BIOS *bios;
    RAM *ram;

public:
    ~Interconnect();

    bool init(SPU *spu, BIOS *bios, RAM *ram);
    void store8(uint32_t address, uint8_t value);
    void store16(uint32_t address, uint16_t value);
    void store32(uint32_t address, uint32_t value);
    uint8_t load8(uint32_t address);
    uint16_t load16(uint32_t address);
    uint32_t load32(uint32_t address);

    bool canLoad32(uint32_t address);
};

#endif /* INTERCONNECT_H */
