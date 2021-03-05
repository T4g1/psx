#ifndef CPU_H
#define CPU_H

#include  <array>
#include  <cstdint>

#define INSTRUCTION_LENGTH  4 // 4 * 8bits = 32 bits
#define DEFAULT_PC          0xBFC00000
#define DEFAULT_REG         0xDEADBEEF
#define REG_COUNT           32
#define RA                  31  // Return address

#define BEV_MASK            0x00400000

#define EXCEPTION_LOAD_ADDRESS_ERROR        0x4
#define EXCEPTION_STORE_ADDRESS_ERROR       0x5
#define EXCEPTION_SYSCALL                   0x8
#define EXCEPTION_OVERFLOW                  0xC

class Interconnect;


/**
 * @brief      CPU for the PSX
 */
class CPU {
    Interconnect *inter;

    // Registers
    std::array<uint32_t, REG_COUNT> reg;
    uint32_t PC;
    uint32_t HI;
    uint32_t LO;

    uint32_t currentPC;     // Set EPC for exceptions
    uint32_t nextPC;

    bool isBranch;          // True if we are branching
    bool isDelaySlot;       // True if we are in a delay slot

    // Emulates load delay: Contains output of current instructions
    std::array<uint32_t, REG_COUNT> out_reg;

    // Pending load (if load_reg != 0 thre is a pending load)
    size_t load_reg = 0;
    uint32_t load_value = 0;

    // COP0 registers
    uint32_t SR;
    uint32_t CAUSE;         // cop0 13: Cause Register
    uint32_t EPC;           // cop0 14: EPC

public:
    ~CPU();

    bool init();
    void reset();
    void run_load();
    void run_next();
    void decode_and_execute(uint32_t data);

    void exception(uint32_t cause);
    void branch(uint32_t offset);

    void set_inter(Interconnect* inter);

    void display_registers();

    uint32_t get_reg(size_t index);
    int32_t get_reg_se(size_t index);
    void set_reg(size_t index, uint32_t value);

    // TESTS
    uint32_t force_get_reg(size_t index);
    void force_set_reg(size_t index, uint32_t value);
    uint32_t get_PC();
    uint32_t get_HI();
    uint32_t get_LO();

    // CPU Opcodes
    void SPECIAL(uint32_t data);
    void BcondZ(size_t rs, size_t rt, int32_t imm16_se);
    void J(uint32_t imm26);
    void JAL(uint32_t imm26);
    void BEQ(size_t rs, size_t rt, int32_t imm16_se);
    void BNE(size_t rs, size_t rt, int32_t imm16_se);
    void BLEZ(size_t rs, int32_t imm16_se);
    void BGTZ(size_t rs, int32_t imm16_se);
    void ADDI(size_t rs, size_t rt, int32_t imm16_se);
    void ADDIU(size_t rs, size_t rt, int32_t imm16_se);
    void SLTI(size_t rs, size_t rt, int32_t imm16_se);
    void SLTIU(size_t rs, size_t rt, int32_t imm16_se);
    void ANDI(size_t rs, size_t rt, uint32_t imm16);
    void ORI(size_t rs, size_t rt, uint16_t imm16);
    void LUI(size_t rt, uint16_t imm16);
    void COP0(uint32_t data);
    void COP1(uint32_t data);
    void COP2(uint32_t data);
    void COP3(uint32_t data);
    void LB(size_t rs, size_t rt, int32_t imm16_se);
    void LW(size_t rs, size_t rt, int32_t imm16_se);
    void LBU(size_t rs, size_t rt, int32_t imm16_se);
    void SB(size_t rs, size_t rt, int32_t imm16_se);
    void SH(size_t rs, size_t rt, int32_t imm16_se);
    void SW(size_t rs, size_t rt, int32_t imm16_se);

    // SPECIAL Opcodes
    void SLL(size_t rt, size_t rd, uint8_t imm5);
    void SRL(size_t rt, size_t rd, uint8_t imm5);
    void SRA(size_t rt, size_t rd, uint8_t imm5);
    void JR(size_t rs);
    void JALR(size_t rs, size_t rd);
    void SYSCALL();
    void MFHI(size_t rd);
    void MTHI(size_t rs);
    void MFLO(size_t rd);
    void MTLO(size_t rs);
    void DIV(size_t rs, size_t rt);
    void DIVU(size_t rs, size_t rt);
    void ADD(size_t rs, size_t rt, size_t rd);
    void ADDU(size_t rs, size_t rt, size_t rd);
    void SUBU(size_t rs, size_t rt, size_t rd);
    void AND(size_t rs, size_t rt, size_t rd);
    void OR(size_t rs, size_t rt, size_t rd);
    void SLT(size_t rs, size_t rt, size_t rd);
    void SLTU(size_t rs, size_t rt, size_t rd);

    // COP0 Opcodes
    void MFC0(size_t rt, size_t rd);
    void MTC0(size_t rt, size_t rd);
    void RFE();
};

#endif /* CPU_H */
