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

    void store32(uint32_t address, uint32_t value);
    uint32_t load32(uint32_t address);
};

#endif /* RAM_H */
