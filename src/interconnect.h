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
    uint32_t load32(uint32_t address);
};

#endif /* INTERCONNECT_H */
