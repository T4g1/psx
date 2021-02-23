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

    void store32(uint32_t address, uint32_t value);
    uint32_t load32(uint32_t address);
};

#endif /* BIOS_H */
