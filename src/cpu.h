#ifndef CPU_H
#define CPU_H

#include  <cstdint>

#define INSTRUCTION_LENGTH  4 // 4 * 8bits = 32 bits
#define DEFAULT_PC          0xBFC00000
#define DEFAULT_REG         0xDEADBEEF
#define REG_COUNT           32

class Interconnect;


/**
 * @brief      CPU for the PSX
 */
class CPU {
    Interconnect *inter;

    // Registers
    uint32_t reg[REG_COUNT];
    uint32_t PC;
    uint32_t HI;
    uint32_t LO;

    uint32_t get_reg(size_t index);
    void set_reg(size_t index, uint32_t value);

    void ORI(size_t rs, size_t rt, uint16_t imm16);
    void LUI(size_t rt, uint16_t imm16);
    void SW(size_t rs, size_t rt, uint16_t imm16);

public:
    ~CPU();

    bool init();
    void reset();
    void run_next();
    void decode_and_execute(uint32_t data);

    void set_inter(Interconnect* inter);

    void display_registers();
};

#endif /* CPU_H */
