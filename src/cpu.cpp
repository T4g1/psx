#include <stdlib.h>
#include <inttypes.h>

#include "log.h"
#include "interconnect.h"
#include "instruction.h"

#include "cpu.h"

#define SR_CACHE_ISOLATION          0x010000

#define BcondZ_BGEZ_MASK            0b00001
#define BcondZ_LINK_MASK            0b10000


CPU::~CPU()
{
}

/**
 * @brief      Initialize the CPU state
 * @return     true in case of success, false otherwise
 */
bool CPU::init()
{
    reset();

    return true;
}

/**
 * @brief      Reset the CPU state
 */
void CPU::reset()
{
    next_instruction = 0x00000000;

    reg[0] = 0;
    out_reg[0] = 0;
    for (size_t i=1; i<REG_COUNT; i++) {
        reg[i] = DEFAULT_REG;
        out_reg[i] = DEFAULT_REG;
    }

    PC = DEFAULT_PC;
    HI = DEFAULT_REG;
    LO = DEFAULT_REG;
}

/**
 * @brief      Exceture pending load if any
 */
void CPU::run_load()
{
    // Execute pending load
    set_reg(load_reg, load_value);
    load_reg = 0;
}

void CPU::run_next()
{
    uint32_t instruction = next_instruction;

    next_instruction = inter->load32(PC);

    PC += INSTRUCTION_LENGTH;

    run_load();

    decode_and_execute(instruction);

    // Set input registers for the next instruction
    reg = out_reg;
}

void CPU::decode_and_execute(uint32_t data)
{
    //debug("[CPU] PC: 0x%08x Instruction: 0x%08x ", PC, data);
    //decode(data);

    uint8_t opcode = get_primary_opcode(data);

    switch(opcode) {
    case 0x00: SPECIAL(data); break;
    case 0x01: BcondZ(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x02: J(get_imm26(data)); break;
    case 0x03: JAL(get_imm26(data)); break;
    case 0x04: BEQ(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x05: BNE(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x06: BLEZ(get_rs(data), get_imm16_se(data)); break;
    case 0x07: BGTZ(get_rs(data), get_imm16_se(data)); break;
    case 0x08: ADDI(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x09: ADDIU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0A: SLTI(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0B: SLTIU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0C: ANDI(get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0D: ORI(get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0F: LUI(get_rt(data), get_imm16(data)); break;
    case 0x10: COP0(data); break;
    case 0x11: COP1(data); break;
    case 0x12: COP2(data); break;
    case 0x13: COP3(data); break;
    case 0x20: LB(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x23: LW(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x24: LBU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x28: SB(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x29: SH(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x2B: SW(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    default:
        error("Unhandled OPCODE: 0x%02x (inst: 0x%08x)\n", opcode, data);
        exit(1);
    }
}

void CPU::set_inter(Interconnect* inter)
{
    this->inter = inter;
}

void CPU::display_registers()
{
    debug("PC: 0x%08x ", PC);
    debug("HI: 0x%08x ", HI);
    debug("LO: 0x%08x\n", LO);

    for (size_t i=0; i<REG_COUNT; i++) {
        debug("$r%zu: 0x%08x\n", i, reg[i]);
    }
}

uint32_t CPU::get_reg(size_t index)
{
    return reg[index];
}

int32_t CPU::get_reg_se(size_t index)
{
    return (int32_t) reg[index];
}

void CPU::set_reg(size_t index, uint32_t value)
{
    out_reg[index] = value;
    out_reg[0] = 0;
}

/**
 * @brief      Skip load delay (used for testing purposes)
 */
uint32_t CPU::force_get_reg(size_t index)
{
    reg = out_reg;
    return get_reg(index);
}

/**
 * @brief      Skip load delay (used for testing purposes)
 */
void CPU::force_set_reg(size_t index, uint32_t value)
{
    set_reg(index, value);
    reg = out_reg;
}

void CPU::branch(uint32_t offset)
{
    PC += (offset << 2);
    PC -= INSTRUCTION_LENGTH;   // Compensate for run_next
}

uint32_t CPU::get_PC()
{
    return PC;
}

uint32_t CPU::get_HI()
{
    return HI;
}

uint32_t CPU::get_LO()
{
    return LO;
}


/******************************************************
 *
 * CPU Opcodes
 *
 ******************************************************/

void CPU::SPECIAL(uint32_t data)
{
    uint8_t opcode = get_secondary_opcode(data);

    switch(opcode) {
    case 0x00: SLL(get_rt(data), get_rd(data), get_imm5(data)); break;
    case 0x02: SRL(get_rt(data), get_rd(data), get_imm5(data)); break;
    case 0x03: SRA(get_rt(data), get_rd(data), get_imm5(data)); break;
    case 0x08: JR(get_rs(data)); break;
    case 0x09: JALR(get_rs(data), get_rd(data)); break;
    case 0x10: MFHI(get_rd(data)); break;
    case 0x12: MFLO(get_rd(data)); break;
    case 0x1A: DIV(get_rs(data), get_rt(data)); break;
    case 0x1B: DIVU(get_rs(data), get_rt(data)); break;
    case 0x20: ADD(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x21: ADDU(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x23: SUBU(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x24: AND(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x25: OR(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x2A: SLT(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x2B: SLTU(get_rs(data), get_rt(data), get_rd(data)); break;
    default:
        error("Unhandled SECONDARY OPCODE: 0x%02x (inst: 0x%08x)\n", opcode, data);
        exit(1);
    }
}

void CPU::BcondZ(size_t rs, size_t rt, int32_t imm16_se)
{
    bool isBGEZ = rt & BcondZ_BGEZ_MASK;
    bool isLink = rt & BcondZ_LINK_MASK;

    bool test = (get_reg_se(rs) < 0);
    if (isBGEZ) {
        test = !test;
    }

    if (test) {
        if (isLink) {
            set_reg(RA, PC);
        }

        branch(imm16_se);
    }
}

void CPU::J(uint32_t imm26)
{
    PC = (PC & 0xF0000000) | (imm26 << 2);
}

void CPU::JAL(uint32_t imm26)
{
    set_reg(RA, PC);

    J(imm26);
}

void CPU::BEQ(size_t rs, size_t rt, int32_t imm16_se)
{
    if (get_reg(rs) == get_reg(rt)) {
        branch(imm16_se);
    }
}

void CPU::BNE(size_t rs, size_t rt, int32_t imm16_se)
{
    if (get_reg(rs) != get_reg(rt)) {
        branch(imm16_se);
    }
}

void CPU::BLEZ(size_t rs, int32_t imm16_se)
{
    if ((int32_t) get_reg(rs) <= 0) {
        branch(imm16_se);
    }
}

void CPU::BGTZ(size_t rs, int32_t imm16_se)
{
    if ((int32_t) get_reg(rs) > 0) {
        branch(imm16_se);
    }
}

void CPU::ADDIU(size_t rs, size_t rt, int32_t imm16_se)
{
    set_reg(rt, get_reg(rs) + imm16_se);
}

void CPU::ADDI(size_t rs, size_t rt, int32_t imm16_se)
{
    uint64_t extended_rs = 0xFFFFFFFF00000000 | get_reg(rs);
    uint64_t result = extended_rs + imm16_se;

    // Overflow!
    if ((result & 0xFFFFFFFF00000000) == 0) {
        error("Unhandled ADDI overflow 0x%016" PRIx64 "\n", result);
        exit(1);
    }

    else {
        set_reg(rt, result);
    }
}

void CPU::SLTI(size_t rs, size_t rt, int32_t imm16_se)
{
    set_reg(rt, get_reg_se(rs) < imm16_se);
}

void CPU::SLTIU(size_t rs, size_t rt, int32_t imm16_se)
{
    set_reg(rt, get_reg(rs) < (uint32_t) imm16_se);
}

void CPU::ANDI(size_t rs, size_t rt, uint32_t imm16)
{
    set_reg(rt, get_reg(rs) & imm16);
}

void CPU::COP0(uint32_t data)
{
    uint8_t opcode = get_cop_opcode(data);

    switch(opcode) {
    case 0b00000: MFC0(get_rt(data), get_rd(data)); break;
    case 0b00100: MTC0(get_rt(data), get_rd(data)); break;
    default:
        error("Unhandled COP0 OPCODE: 0x%02x\n", opcode);
        exit(1);
    }
}

void CPU::COP1(uint32_t data)
{
    error("Unhandled COP1: 0x%08x", data);
    exit(1);
}

void CPU::COP2(uint32_t data)
{
    error("Unhandled COP2: 0x%08x", data);
    exit(1);
}

void CPU::COP3(uint32_t data)
{
    error("Unhandled COP3: 0x%08x", data);
    exit(1);
}

void CPU::LB(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring load while cache is isolated\n");
        return;
    }

    // Cast for sign extension
    int8_t value = (int8_t)inter->load8(get_reg(rs) + imm16_se);

    // Create a pending load
    load_reg = rt;
    load_value = (uint32_t) value;
}

void CPU::LW(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring load while cache is isolated\n");
        return;
    }

    // Create a pending load
    load_reg = rt;
    load_value = inter->load32(get_reg(rs) + imm16_se);
}

void CPU::LBU(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring load while cache is isolated\n");
        return;
    }

    // Create a pending load
    load_reg = rt;
    load_value = (uint32_t) inter->load8(get_reg(rs) + imm16_se);
}

void CPU::ORI(size_t rs, size_t rt, uint16_t imm16)
{
    set_reg(rt, get_reg(rs) | imm16);
}

void CPU::LUI(size_t rt, uint16_t imm16)
{
    set_reg(rt, imm16 << 16);
}

void CPU::SW(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store32 while cache is isolated\n");
        return;
    }

    inter->store32(get_reg(rs) + imm16_se, get_reg(rt));
}

void CPU::SH(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store16 while cache is isolated\n");
        return;
    }

    inter->store16(get_reg(rs) + imm16_se, (uint16_t) get_reg(rt));
}

void CPU::SB(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store8 while cache is isolated\n");
        return;
    }

    inter->store8(get_reg(rs) + imm16_se, (uint8_t) get_reg(rt));
}


/******************************************************
 *
 * SPECIAL Opcodes
 *
 ******************************************************/

void CPU::SLL(size_t rt, size_t rd, uint8_t imm5)
{
    set_reg(rd, get_reg(rt) << imm5);
}

void CPU::SRL(size_t rt, size_t rd, uint8_t imm5)
{
    set_reg(rd, get_reg(rt) >> imm5);
}

void CPU::SRA(size_t rt, size_t rd, uint8_t imm5)
{
    set_reg(rd, get_reg_se(rt) >> imm5);
}

void CPU::JR(size_t rs)
{
    PC = get_reg(rs);
}

void CPU::JALR(size_t rs, size_t rd)
{
    set_reg(rd, PC);

    JR(rs);
}

void CPU::MFHI(size_t rd)
{
    set_reg(rd, HI);
}

void CPU::MFLO(size_t rd)
{
    set_reg(rd, LO);
}

void CPU::DIV(size_t rs, size_t rt)
{
    int32_t numerator = get_reg_se(rs);
    int32_t denominator = get_reg_se(rt);

    // Division by zero
    if (denominator == 0) {
        HI = numerator;

        if (numerator >= 0) {
            LO = 0xFFFFFFFF;
        } else {
            LO = 1;
        }
    }
    // Result is not 32bit
    else if (numerator == (int32_t) 0x80000000 && denominator == -1) {
        HI = 0;
        LO = 0x80000000;
    }
    else {
        HI = (uint32_t)(numerator % denominator);
        LO = (uint32_t)(numerator / denominator);
    }
}

void CPU::DIVU(size_t rs, size_t rt)
{
    uint32_t numerator = get_reg(rs);
    uint32_t denominator = get_reg(rt);

    // Division by zero
    if (denominator == 0) {
        HI = numerator;
        LO = 0xFFFFFFFF;
    }
    else {
        HI = numerator % denominator;
        LO = numerator / denominator;
    }
}

void CPU::ADD(size_t rs, size_t rt, size_t rd)
{
    int32_t s = get_reg(rs);
    int32_t t = get_reg(rt);

    int32_t result = s + t;

    if ((s > 0 && t > 0 && result < 0) ||
        (s < 0 && t < 0 && result > 0)) {
        error("Unhandled ADD overflow\n");
    }

    set_reg(rd, (uint32_t) result);
}

void CPU::ADDU(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) + get_reg(rt));
}

void CPU::SUBU(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) - get_reg(rt));
}

void CPU::AND(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) & get_reg(rt));
}

void CPU::OR(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) | get_reg(rt));
}

void CPU::SLT(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg_se(rs) < get_reg_se(rt));
}

void CPU::SLTU(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) < get_reg(rt));
}


/******************************************************
 *
 * COP0 Opcodes
 *
 ******************************************************/

void CPU::MFC0(size_t rt, size_t rd)
{
    // Create a pending load
    load_reg = rt;

    switch(rd) {
    case 12:
        load_value = SR;
        break;
    case 13:
        error("Unhandled read COP0 CAUSE\n");
        exit(1);
    default:
        error("Unhandled read COP0 register: %zu\n", rd);
        exit(1);
    }
}

void CPU::MTC0(size_t rt, size_t rd)
{
    uint32_t value = get_reg(rt);

    switch(rd) {
    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
    case 11:
        if (value != 0) {
            error("Unhandled write to COP0 R%zu\n", rd);
            exit(1);
        }
        break;
    case 12:
        SR = get_reg(rt);
        break;
    case 13:
        if (value != 0) {
            error("Unhandled write COP0 CAUSE\n");
            exit(1);
        }
        break;
    default:
        error("Unhandled write COP0 register: %zu\n", rd);
        exit(1);
    }
}
