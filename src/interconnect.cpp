#include "interconnect.h"

#include "log.h"
#include "bios.h"
#include "ram.h"
#include "common.h"

#define RAM_START               0xA0000000

#define BIOS_START              0xBFC00000
#define MEM_CONTROL_START       0x1F801000
#define MEM_CONTROL_SIZE        36

#define EXPANSION_1_BASE_ADDR   0x1F000000
#define EXPANSION_2_BASE_ADDR   0x1F802000

#define RAM_SIZE_START          0x1F801060
#define RAM_SIZE_SIZE           4

#define CACHE_CONTROL_START     0xFFFE0130
#define CACHE_CONTROL_SIZE      4


Interconnect::~Interconnect()
{
}


bool Interconnect::init(BIOS *bios, RAM *ram)
{
    this->bios = bios;
    this->ram = ram;

    return true;
}


void Interconnect::store32(uint32_t address, uint32_t value)
{
    // Unaligned memory access should be handled differently
    if (address % 4 != 0) {
        error("Unaligned store32 at 0x%08x\n", address);
        exit(1);
    }

    //debug("[MEM] Store to: 0x%08x\n", address);

    // Is it mapped to RAM ?
    if (in_range(address, RAM_START, RAM_SIZE)) {
        ram->store32(address - RAM_START, value);
    }

    // Is it mapped to BIOS ?
    else if (in_range(address, BIOS_START, BIOS_SIZE)) {
        bios->store32(address - BIOS_START, value);
    }

    // Is it mapped to EXPANSION 1 or 2 ?
    else if (in_range(address, MEM_CONTROL_START, MEM_CONTROL_SIZE)) {
        uint32_t offset = address - MEM_CONTROL_START;

        switch(offset) {
        case 0:
            if (value != EXPANSION_1_BASE_ADDR) {
                error("Bad expansion 1 base address 0x%08x\n", value);
                exit(1);
            }
            break;
        case 4:
            if (value != EXPANSION_2_BASE_ADDR) {
                error("Bad expansion 2 base address 0x%08x\n", value);
                exit(1);
            }
            break;
        default:
            error("Unhandled write to MEM_CONTROL register: 0x%08x: 0x%08x\n", offset, value);
            //exit(1);
            break;
        }
    }

    // RAM_SIZE for RAM configuration
    else if (in_range(address, RAM_SIZE_START, RAM_SIZE_SIZE)) {
        uint32_t offset = address - RAM_SIZE_START;
        error("Unhandled write to RAM_SIZE register: 0x%08x: 0x%08x\n", offset, value);
    }

    // CACHE_CONTROL register
    else if (in_range(address, CACHE_CONTROL_START, CACHE_CONTROL_SIZE)) {
        uint32_t offset = address - CACHE_CONTROL_START;
        error("Unhandled write to CACHE_CONTROL register: 0x%08x: 0x%08x\n", offset, value);
    }

    else {
        error("Unhandled store32 at 0x%08x\n", address);
        exit(1);
    }
}


uint32_t Interconnect::load32(uint32_t address)
{
    // Unaligned memory access should be handled differently
    if (address % 4 != 0) {
        error("Unaligned load32 at 0x%08x\n", address);
        exit(1);
    }

    //debug("[MEM] Load from: 0x%08x\n", address);

    // Is it mapped to RAM ?
    if (in_range(address, RAM_START, RAM_SIZE)) {
        return ram->load32(address - RAM_START);
    }

    // Is it mapped to BIOS ?
    else if (in_range(address, BIOS_START, BIOS_SIZE)) {
        return bios->load32(address - BIOS_START);
    }

    else {
        error("Unhandled load32 at 0x%08x\n", address);
        exit(1);
    }
}
