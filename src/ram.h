#ifndef RAM_H
#define RAM_H

#include <cstdint>

#define RAM_SIZE            2048 * 1024


/**
 * @brief      RAM for the PSX
 */
class RAM {
    uint8_t data[RAM_SIZE];

public:
    ~RAM();

    bool init();

    void store8(uint32_t address, uint8_t value);
    void store16(uint32_t address, uint16_t value);
    void store32(uint32_t address, uint32_t value);
    uint8_t load8(uint32_t address);
    uint16_t load16(uint32_t address);
    uint32_t load32(uint32_t address);
};

#endif /* RAM_H */
