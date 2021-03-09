#ifndef RAM_H
#define RAM_H

#include <cstdint>

#include "common.h"


#define POISON_VALUE        0xCA

#define RAM_SIZE            2048 * 1024


/**
 * @brief      RAM for the PSX
 */
class RAM {
    uint8_t data[RAM_SIZE];

public:
    ~RAM();

    bool init();

    template<typename T>
    void store(uint32_t address, T value)
    {
        for (size_t i=0; i<sizeof(T); i++) {
            data[address + i] = extract(value, i * 8, 8);
        }
    }

    template<typename T>
    T load(uint32_t address)
    {
        T value = 0;

        for (size_t i=0; i<sizeof(T); i++) {
            uint8_t byte = data[address + i];

            value |= byte << (i * 8);
        }

        return value;
    }
};

#endif /* RAM_H */
