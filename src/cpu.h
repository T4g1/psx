#ifndef CPU_H
#define CPU_H

#include  <cstdint>

#define INSTRUCTION_LENGTH  4 // 32 bits

/**
 * @brief      CPU for the PSX
 */
class CPU {
    // Registers
    uint32_t PC;

public:
    ~CPU();

    bool init();
    void reset();
    void run_next();
    void decode_and_execute(uint32_t data);
};

#endif /* CPU_H */
