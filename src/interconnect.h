#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <cstdint>

#include "log.h"
#include "spu.h"
#include "bios.h"
#include "ram.h"
#include "common.h"

#define RAM_START               0x00000000

#define BIOS_START              0x1FC00000

#define SYS_CONTROL_START       0x1F801000
#define SYS_CONTROL_SIZE        36

#define RAM_SIZE_START          0x1F801060
#define RAM_SIZE_SIZE           4

#define SPU_START               0x1F801C00
#define SPU_SIZE                640

// KSEG2
#define CACHE_CONTROL_START     0xFFFE0130
#define CACHE_CONTROL_SIZE      4

#define EXPANSION_1_START       0x1F000000
#define EXPANSION_1_SIZE        8192 * 1024

#define IRQ_CONTROL_START       0x1F801070
#define IRQ_CONTROL_SIZE        8

#define DMA_START               0x1F801080
#define DMA_SIZE                128

#define TIMERS_START            0x1F801100
#define TIMERS_SIZE             16 * 3 // 3 timers

#define GPU_START               0x1F801810
#define GPU_SIZE                2 * 4

#define EXPANSION_2_START       0x1F802000
#define EXPANSION_2_SIZE        66

class SPU;
class BIOS;
class RAM;


uint32_t mask_region(uint32_t address);


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

    bool canLoad32(uint32_t address);

    template <typename T>
    void store(uint32_t address, T value)
    {
        address = mask_region(address);

        if ((sizeof(T) == sizeof(uint16_t) && address % 2 != 0) ||
            (sizeof(T) == sizeof(uint32_t) && address % 4 != 0))
         {
            error("Unaligned store%lld at 0x%08x\n", sizeof(T), address);
            exit(1);
        }

        // Is it mapped to RAM ?
        if (in_range(address, RAM_START, RAM_SIZE)) {
            ram->store<T>(address - RAM_START, value);
        }

        // Is it mapped to BIOS ?
        else if (in_range(address, BIOS_START, BIOS_SIZE)) {
            error("Unhandled store%lld to BIOS 0x%08x (read only!)\n", sizeof(T), address);
            exit(1);
        }

        // Is it mapped to EXPANSION 1 or 2 ?
        else if (in_range(address, SYS_CONTROL_START, SYS_CONTROL_SIZE)) {
            uint32_t offset = address - SYS_CONTROL_START;

            switch(offset) {
            case 0:
                if (value != EXPANSION_1_START) {
                    error("Bad expansion 1 base address 0x%08x\n", value);
                    exit(1);
                }
                break;
            case 4:
                if (value != EXPANSION_2_START) {
                    error("Bad expansion 2 base address 0x%08x\n", value);
                    exit(1);
                }
                break;
            default:
                error("Unhandled store32 to MEM_CONTROL register: 0x%08x: 0x%08x\n", offset, value);
                //exit(1);
                break;
            }
        }

        // RAM_SIZE for RAM configuration
        else if (in_range(address, RAM_SIZE_START, RAM_SIZE_SIZE)) {
            uint32_t offset = address - RAM_SIZE_START;
            error("Unhandled store%lld to RAM_SIZE register: 0x%08x: 0x%08x\n", sizeof(T), offset, value);
        }

        // Is it mapped to SPU ?
        else if (in_range(address, SPU_START, SPU_SIZE)) {
            //uint32_t offset = address - SPU_START;
            //error("Unhandled store%lld to SPU register: 0x%08x: 0x%04x\n", sizeof(T), offset, value);
        }

        // CACHE_CONTROL register
        else if (in_range(address, CACHE_CONTROL_START, CACHE_CONTROL_SIZE)) {
            uint32_t offset = address - CACHE_CONTROL_START;
            error("Unhandled store%lld to CACHE_CONTROL register: 0x%08x: 0x%08x\n", sizeof(T), offset, value);
        }

        // IRQ_CONTROL register
        else if (in_range(address, IRQ_CONTROL_START, IRQ_CONTROL_SIZE)) {
            uint32_t offset = address - IRQ_CONTROL_START;
            error("Unhandled store%lld to IRQ_CONTROL register: 0x%08x: 0x%04x\n", sizeof(T), offset, value);
        }

        // Is it mapped to DMA ?
        else if (in_range(address, DMA_START, DMA_SIZE)) {
            uint32_t offset = address - DMA_START;
            error("Unhandled store%lld to DMA register: 0x%08x: 0x%08x\n", sizeof(T), offset, value);
        }

        // Is it mapped to TIMERS ?
        else if (in_range(address, TIMERS_START, TIMERS_SIZE)) {
            uint32_t offset = address - TIMERS_START;

            error("Unhandled store%lld to TIMERS register: 0x%08x: 0x%04x\n", sizeof(T), offset, value);
        }

        // Is it mapped to GPU ?
        else if (in_range(address, GPU_START, GPU_SIZE)) {
            uint32_t offset = address - GPU_START;
            error("Unhandled store%lld to GPU register: 0x%08x: 0x%08x\n", sizeof(T), offset, value);
        }

        // Is it mapped to EXPANSION 2 ?
        else if (in_range(address, EXPANSION_2_START, EXPANSION_2_SIZE)) {
            uint32_t offset = address - EXPANSION_2_START;

            error("Unhandled store%lld to EXPANSION 2 register: 0x%08x: 0x%02x\n", sizeof(T), offset, value);
        }

        else {
            error("Unhandled store%lld at 0x%08x\n", sizeof(T), address);
            exit(1);
        }
    }

    template <typename T>
    T load(uint32_t address)
    {
        address = mask_region(address);

        // Is it mapped to RAM ?
        if (in_range(address, RAM_START, RAM_SIZE)) {
            return ram->load<T>(address - RAM_START);
        }

        // Is it mapped to BIOS ?
        else if (in_range(address, BIOS_START, BIOS_SIZE)) {
            return bios->load<T>(address - BIOS_START);
        }

        // Is it mapped to SPU ?
        else if (in_range(address, SPU_START, SPU_SIZE)) {
            //uint32_t offset = address - SPU_START;
            //error("Unhandled load%lld to SPU register: 0x%08x\n", sizeof(T), offset);
            return 0;
        }

        // Is it mapped to EXPANSION 1 ?
        else if (in_range(address, EXPANSION_1_START, EXPANSION_1_SIZE)) {
            return (T) 0xFFFFFFFF;
        }

        // IRQ_CONTROL register
        else if (in_range(address, IRQ_CONTROL_START, IRQ_CONTROL_SIZE)) {
            uint32_t offset = address - IRQ_CONTROL_START;
            error("Unhandled load%lld to IRQ_CONTROL register: 0x%08x\n", sizeof(T), offset);
            return 0;
        }

        // Is it mapped to DMA ?
        else if (in_range(address, DMA_START, DMA_SIZE)) {
            uint32_t offset = address - DMA_START;
            error("Unhandled load%lld to DMA register: 0x%08x\n", sizeof(T), offset);
            return 0;
        }

        // Is it mapped to GPU ?
        else if (in_range(address, GPU_START, GPU_SIZE)) {
            uint32_t offset = address - GPU_START;

            switch(offset) {
            // Let the CPU knows GPU is ready
            case 4: return (T) 0x10000000;
            default:
                error("Unhandled load%lld to GPU register: 0x%08x\n", sizeof(T), offset);
                return 0;
            }
        }

        else {
            error("Unhandled load%lld at 0x%08x\n", sizeof(T), address);
            exit(1);
        }
    }
};

#endif /* INTERCONNECT_H */
