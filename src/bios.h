#ifndef BIOS_H
#define BIOS_H

#include <string>
#include <cstdint>

#define BIOS_SIZE   512 * 1024 // 512KB


/**
 * @brief      BIOS class able to load any bios ROM file
 */
class BIOS {
    uint8_t data[BIOS_SIZE];

public:
    ~BIOS();

    bool init(std::string bios_path);
    bool load_bios(std::string path);

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

#endif /* BIOS_H */
