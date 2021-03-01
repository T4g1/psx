#ifndef CPU_H
#define CPU_H

#include  <array>
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

    // Used to store next instructions so JUMPS behave correctly
    uint32_t next_instruction;

    // Registers
    std::array<uint32_t, REG_COUNT> reg;
    uint32_t PC;
    uint32_t HI;
    uint32_t LO;

    // Emulates load delay: Contains output of current instructions
    std::array<uint32_t, REG_COUNT> out_reg;

    // Pending load (if load_reg != 0 thre is a pending load)
    size_t load_reg = 0;
    uint32_t load_value = 0;

    // COP0 registers
    uint32_t SR;

public:
    ~CPU();

    bool init();
    void reset();
    void run_next();
    void decode_and_execute(uint32_t data);

    void set_inter(Interconnect* inter);

    void display_registers();

    uint32_t get_reg(size_t index);
    void set_reg(size_t index, uint32_t value);

    void branch(uint32_t offset);

    void SPECIAL(uint32_t data);
    void J(uint32_t imm26);
    void BNE(size_t rs, size_t rt, uint32_t imm16_se);
    void ADDI(size_t rs, size_t rt, uint32_t imm16_se);
    void ADDIU(size_t rs, size_t rt, uint32_t imm16_se);
    void COP0(uint32_t data);
    void COP1(uint32_t data);
    void COP2(uint32_t data);
    void COP3(uint32_t data);
    void LW(size_t rs, size_t rt, uint32_t imm16_se);
    void ORI(size_t rs, size_t rt, uint16_t imm16);
    void LUI(size_t rt, uint16_t imm16);
    void SW(size_t rs, size_t rt, uint32_t imm16_se);
    void SH(size_t rs, size_t rt, uint32_t imm16_se);

    void SLL(size_t rt, size_t rd, uint8_t imm5);
    void ADDU(size_t rs, size_t rt, size_t rd);
    void OR(size_t rs, size_t rt, size_t rd);
    void SLTU(size_t rs, size_t rt, size_t rd);

    void MTC0(size_t rt, size_t rd);
};

#endif /* CPU_H */
