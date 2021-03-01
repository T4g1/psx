#include <stdlib.h>

#include "log.h"
#include "interconnect.h"
#include "instruction.h"

#include "cpu.h"

#define SR_CACHE_ISOLATION          0x010000


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


void CPU::run_next()
{
    uint32_t instruction = next_instruction;

    next_instruction = inter->load32(PC);

    PC += INSTRUCTION_LENGTH;

    // Execute pending load
    set_reg(load_reg, load_value);
    load_reg = 0;

    decode_and_execute(instruction);

    // Set input registers for the next instruction
    reg = out_reg;
}


void CPU::decode_and_execute(uint32_t data)
{
    //debug("[CPU] PC: 0x%08x Instruction: 0x%08x\n", PC, data);

    uint8_t opcode = get_primary_opcode(data);

    switch(opcode) {
    case 0x00: SPECIAL(data); break;
    case 0x02: J(get_imm26(data)); break;
    case 0x05: BNE(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x08: ADDI(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x09: ADDIU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x10: COP0(data); break;
    case 0x11: COP1(data); break;
    case 0x12: COP2(data); break;
    case 0x13: COP3(data); break;
    case 0x23: LW(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x0D: ORI(get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0F: LUI(get_rt(data), get_imm16(data)); break;
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


void CPU::set_reg(size_t index, uint32_t value)
{
    out_reg[index] = value;
    out_reg[0] = 0;
}


void CPU::branch(uint32_t offset)
{
    PC += offset;
    PC -= INSTRUCTION_LENGTH;   // Compensate for run_next
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
    case 0x25: OR(get_rs(data), get_rt(data), get_rd(data)); break;
    default:
        error("Unhandled SECONDARY OPCODE: 0x%02x", opcode);
        exit(1);
    }
}

void CPU::J(uint32_t imm26)
{
    PC = (PC & 0xF0000000) | (imm26 << 2);
}

void CPU::BNE(size_t rs, size_t rt, uint32_t imm16_se)
{
    if (get_reg(rs) != get_reg(rt)) {
        branch(imm16_se << 2);
    }
}

void CPU::ADDIU(size_t rs, size_t rt, uint32_t imm16_se)
{
    set_reg(rt, get_reg(rs) + imm16_se);
}

void CPU::ADDI(size_t rs, size_t rt, uint32_t imm16_se)
{
    uint64_t extended_rs = 0xFFFFFFFF00000000 | get_reg(rs);
    uint64_t result = extended_rs + imm16_se;

    // Overflow!
    if ((result & 0xFFFFFFFF00000000) == 0) {
        error("Unhandled ADDI overflow\n");
        exit(1);
    }

    else {
        set_reg(rt, result);
    }
}

void CPU::COP0(uint32_t data)
{
    uint8_t opcode = get_cop_opcode(data);

    switch(opcode) {
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

void CPU::LW(size_t rs, size_t rt, uint32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring load while cache is isolated\n");
        return;
    }

    // Create a pending load
    load_reg = rt;
    load_value = inter->load32(get_reg(rs) + imm16_se);
}

void CPU::ORI(size_t rs, size_t rt, uint16_t imm16)
{
    set_reg(rt, get_reg(rs) | imm16);
}

void CPU::LUI(size_t rt, uint16_t imm16)
{
    set_reg(rt, imm16 << 16);
}

void CPU::SW(size_t rs, size_t rt, uint32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store while cache is isolated\n");
        return;
    }

    inter->store32(get_reg(rs) + imm16_se, get_reg(rt));
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

void CPU::OR(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) | get_reg(rt));
}


/******************************************************
 *
 * COP0 Opcodes
 *
 ******************************************************/

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
            error("Unhandled write to COP0 CAUSE\n");
            exit(1);
        }
        break;
    default:
        error("Unhandled COP0 register: %zu\n", rd);
        exit(1);
    }
}
