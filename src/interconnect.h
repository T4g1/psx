#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <cstdint>

class BIOS;


/**
 * @brief      Handles virtual memory mapping
 * Dispatch read/write request to correct modules and/or memory
 */
class Interconnect {
    BIOS *bios;

public:
    ~Interconnect();

    bool init(BIOS *bios);
    void store32(uint32_t address, uint32_t value);
    uint32_t load32(uint32_t address);
};

#endif /* INTERCONNECT_H */
