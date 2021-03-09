#include "cpu.h"

#include <stdlib.h>
#include <inttypes.h>

#include "imgui.h"

#include "log.h"
#include "instruction.h"
#include "common.h"


#define SR_CACHE_ISOLATION          0x010000

#define BcondZ_BGEZ_MASK            0b00001
#define BcondZ_LINK_MASK            0b10000

#define EXEC_STACK_SIZE             50


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
    reg[0] = 0;
    out_reg[0] = 0;
    for (size_t i=1; i<REG_COUNT; i++) {
        reg[i] = DEFAULT_REG;
        out_reg[i] = DEFAULT_REG;
    }

    PC = DEFAULT_PC;
    nextPC = DEFAULT_PC + INSTRUCTION_LENGTH;
    HI = DEFAULT_REG;
    LO = DEFAULT_REG;

    isBranch = false;
    isDelaySlot = false;
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
    currentPC = PC; // Used to set EPC in case of exception
    if (currentPC % 4 != 0) {
        exception(EXCEPTION_LOAD_ADDRESS_ERROR);
        return;
    }

    uint32_t instruction = load<uint32_t>(PC);

    PC = nextPC;
    nextPC += INSTRUCTION_LENGTH;

    run_load();

    // If we where branching, then we are in the delay slot now
    isDelaySlot = isBranch;
    isBranch = false;

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
    case 0x0E: XORI(get_rs(data), get_rt(data), get_imm16(data)); break;
    case 0x0F: LUI(get_rt(data), get_imm16(data)); break;
    case 0x10: COP0(data); break;
    case 0x11: COP1(); break;
    case 0x12: COP2(data); break;
    case 0x13: COP3(); break;
    case 0x20: LB(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x21: LH(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x22: LWL(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x23: LW(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x24: LBU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x25: LHU(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x26: LWR(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x28: SB(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x29: SH(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x2A: SWL(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x2B: SW(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x2E: SWR(get_rs(data), get_rt(data), get_imm16_se(data)); break;
    case 0x30: LWC0(); break;
    case 0x31: LWC1(); break;
    case 0x32: LWC2(data); break;
    case 0x33: LWC3(); break;
    case 0x38: SWC0(); break;
    case 0x39: SWC1(); break;
    case 0x3A: SWC2(data); break;
    case 0x3B: SWC3(); break;
    default: exception(EXCEPTION_ILLEGAL_INSTRUCTIONS); break;
    }
}

void CPU::exception(uint32_t cause)
{
    // Handler depends on BEV bit in SR
    uint32_t handler = 0x80000080;
    if (SR & BEV_MASK) {
        handler = 0xBFC00180;
    }

    // Last 6 bit of SR is three pairs of Interupt Enable/User mode bits
    // They behave like a stack of 3 entries. They are pushed with two zero
    // at each exception.
    uint32_t mode = SR & MASK_6_BITS;  // Store last 6 bits of SR
    SR &= ~MASK_6_BITS;                // Clear 6 last bits
    SR |= (mode << 2) & MASK_6_BITS;   // Shift mode and store back in SR

    // CAUSE register updated with exception code (bits [6:2])
    CAUSE = cause << 2;
    EPC = currentPC;

    if (isDelaySlot) {
        // Set bit 31 of cause and set EPC to the branch instruction
        // when exception occurs during a delay slot
        EPC -= INSTRUCTION_LENGTH;
        CAUSE |= 1 << 31;
    }

    // No delay slot for exceptions
    PC = handler;
    nextPC = PC + INSTRUCTION_LENGTH;
}

void CPU::branch(uint32_t offset)
{
    isBranch = true;
    nextPC = PC + (offset << 2);
}

void CPU::set_inter(Interconnect* inter)
{
    this->inter = inter;
}

void CPU::print_registers()
{
    debug("PC: 0x%08x ", PC);
    debug("Next PC: 0x%08x ", nextPC);
    debug("HI: 0x%08x ", HI);
    debug("LO: 0x%08x\n", LO);

    for (size_t i=0; i<REG_COUNT; i++) {
        debug("$r%zu: 0x%08x\n", i, reg[i]);
    }
}

void CPU::display_registers(bool *status)
{
    const char *title = "Registers";

    if (ImGui::Begin(title, status)) {
        ImGui::BeginChild("registers");

        ImGui::Text("PC: 0x%08X", PC);
        ImGui::Separator();
        ImGui::Text("HI: 0x%08X LOW: 0x%08X", HI, LO);
        ImGui::Text("SR: 0x%08X", SR);
        ImGui::Text("CAUSE: 0x%08X EPC: 0x%08X", CAUSE, EPC);
        ImGui::Separator();
        for (size_t i=0; i<REG_COUNT; i++) {
            ImGui::Text("R%02zu: 0x%08x OUT_R%02zu: 0x%08x", i, reg[i], i, out_reg[i]);
        }
        ImGui::Separator();

        ImGui::Columns(2, "boolean", false);

        ImGui::Text("Branch?");
        ImGui::NextColumn();
        ColorBoolean(isBranch);
        ImGui::NextColumn();

        ImGui::Text("Delay slot?");
        ImGui::NextColumn();
        ColorBoolean(isDelaySlot);
        ImGui::NextColumn();

        ImGui::Columns(1, "boolean", false);

        ImGui::EndChild();
    }

    ImGui::End();
}

void CPU::display_execution(bool *status)
{
    const char *title = "Execution";
    char buffer[INSTRUCTION_MAX_SIZE];

    if (ImGui::Begin(title, status)) {
        ImGui::BeginChild("execution");

        for (int i=-EXEC_STACK_SIZE; i<EXEC_STACK_SIZE; i++) {
            if (i == 0) {
                ImGui::Separator();
            }

            uint32_t address = PC + (i * INSTRUCTION_LENGTH);
            if (inter->canLoad32(address)) {
                uint32_t data = load<uint32_t>(address);

                decode(buffer, INSTRUCTION_MAX_SIZE, data);
                ImGui::Text("0x%08X %s", data, buffer);
            } else {
                ImGui::Text("0x???????? ?");
            }

            if (i == 0) {
                ImGui::Separator();
            }
        }

        ImGui::EndChild();
    }

    ImGui::End();
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


/******************************************************
 *
 * TESTS
 *
 ******************************************************/

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
    case 0x04: SLLV(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x06: SRLV(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x07: SRAV(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x08: JR(get_rs(data)); break;
    case 0x09: JALR(get_rs(data), get_rd(data)); break;
    case 0x0C: SYSCALL(); break;
    case 0x0D: BREAK(); break;
    case 0x10: MFHI(get_rd(data)); break;
    case 0x11: MTHI(get_rs(data)); break;
    case 0x12: MFLO(get_rd(data)); break;
    case 0x13: MTLO(get_rs(data)); break;
    case 0x18: MULT(get_rs(data), get_rt(data)); break;
    case 0x19: MULTU(get_rs(data), get_rt(data)); break;
    case 0x1A: DIV(get_rs(data), get_rt(data)); break;
    case 0x1B: DIVU(get_rs(data), get_rt(data)); break;
    case 0x20: ADD(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x21: ADDU(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x22: SUB(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x23: SUBU(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x24: AND(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x25: OR(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x26: XOR(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x27: NOR(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x2A: SLT(get_rs(data), get_rt(data), get_rd(data)); break;
    case 0x2B: SLTU(get_rs(data), get_rt(data), get_rd(data)); break;
    default: exception(EXCEPTION_ILLEGAL_INSTRUCTIONS); break;
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
            set_reg(RA, nextPC);
        }

        branch(imm16_se);
    }
}

void CPU::J(uint32_t imm26)
{
    isBranch = true;
    nextPC = (PC & 0xF0000000) | (imm26 << 2);
}

void CPU::JAL(uint32_t imm26)
{
    set_reg(RA, nextPC);

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

void CPU::ADDI(size_t rs, size_t rt, int32_t imm16_se)
{
    uint64_t extended_rs = 0xFFFFFFFF00000000 | get_reg(rs);
    uint64_t result = extended_rs + imm16_se;

    // Overflow!
    if ((result & 0xFFFFFFFF00000000) == 0) {
        exception(EXCEPTION_OVERFLOW);
    }

    else {
        set_reg(rt, result);
    }
}

void CPU::ADDIU(size_t rs, size_t rt, int32_t imm16_se)
{
    set_reg(rt, get_reg(rs) + imm16_se);
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

void CPU::ORI(size_t rs, size_t rt, uint16_t imm16)
{
    set_reg(rt, get_reg(rs) | imm16);
}

void CPU::XORI(size_t rs, size_t rt, uint16_t imm16)
{
    set_reg(rt, get_reg(rs) ^ imm16);
}

void CPU::LUI(size_t rt, uint16_t imm16)
{
    set_reg(rt, imm16 << 16);
}

void CPU::COP0(uint32_t data)
{
    uint8_t opcode = get_cop_opcode(data);              // Bits 25 - 21
    uint8_t sec_opcode = get_secondary_opcode(data);    // Bits 5 - 0

    switch(opcode) {
    case 0b00000: MFC0(get_rt(data), get_rd(data)); break;
    case 0b00100: MTC0(get_rt(data), get_rd(data)); break;
    case 0b10000:
        switch(sec_opcode) {
        case 0b010000: RFE(); break;
        default:
            error("Invalid COP0 Instruction: 0x%02x (inst: 0x%08x)\n", sec_opcode, data);
            exit(1);
        }
        break;
    default:
        error("Unhandled COP0 OPCODE: 0x%02x (inst: 0x%08x)\n", opcode, data);
        exit(1);
    }
}

void CPU::COP1()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::COP2(uint32_t data)
{
    error("Unhandled COP2 GTE: 0x%08x", data);
    exit(1);
}

void CPU::COP3()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::LB(size_t rs, size_t rt, int32_t imm16_se)
{
    // Cast for sign extension
    int8_t value = (int8_t) load<uint8_t>(get_reg(rs) + imm16_se);

    // Create a pending load
    load_reg = rt;
    load_value = (uint32_t) value;
}

void CPU::LH(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;
    if (address % 2 != 0) {
        exception(EXCEPTION_LOAD_ADDRESS_ERROR);
    } else {
        int16_t value = (int16_t) load<uint16_t>(address);

        // Create a pending load
        load_reg = rt;
        load_value = (uint32_t) value;
    }
}

void CPU::LWL(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;

    // Bypass load delay
    uint32_t value = out_reg[rt];

    // Load the word containing the left part the unaligned addressed word
    uint32_t aligned_address = address & ~0x00000003; // Clear two last bits
    uint32_t aligned_value = load<uint32_t>(aligned_address);

    // Depending on the address alignement, store the relevant left part in the
    // loaded value
    switch(address & 0x03) {
    case 0: value = (value & 0x00FFFFFF) | (aligned_value << 24); break;
    case 1: value = (value & 0x0000FFFF) | (aligned_value << 16); break;
    case 2: value = (value & 0x000000FF) | (aligned_value << 8); break;
    case 3: value = (value & 0x00000000) | (aligned_value << 0); break;
    default: exit(1); break; // Unreachable
    }

    // Pending load
    load_reg = rt;
    load_value = value;
}

void CPU::LW(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;
    if (address % 4 != 0) {
        exception(EXCEPTION_LOAD_ADDRESS_ERROR);
    } else {
        // Create a pending load
        load_reg = rt;
        load_value = load<uint32_t>(address);
    }
}

void CPU::LBU(size_t rs, size_t rt, int32_t imm16_se)
{
    // Create a pending load
    load_reg = rt;
    load_value = (uint32_t) load<uint8_t>(get_reg(rs) + imm16_se);
}

void CPU::LHU(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;
    if (address % 2 != 0) {
        exception(EXCEPTION_LOAD_ADDRESS_ERROR);
    } else {
        // Create a pending load
        load_reg = rt;
        load_value = (uint32_t) load<uint16_t>(address);
    }
}

void CPU::LWR(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;

    // Bypass load delay
    uint32_t value = out_reg[rt];

    // Load the word containing the left part the unaligned addressed word
    uint32_t aligned_address = address & ~0x00000003; // Clear two last bits
    uint32_t aligned_value = load<uint32_t>(aligned_address);

    // Depending on the address alignement, store the relevant right part in the
    // loaded value
    switch(address & 0x03) {
    case 0: value = (value & 0x00000000) | (aligned_value >> 0); break;
    case 1: value = (value & 0xFF000000) | (aligned_value >> 8); break;
    case 2: value = (value & 0xFFFF0000) | (aligned_value >> 16); break;
    case 3: value = (value & 0xFFFFFF00) | (aligned_value >> 24); break;
    default: exit(1); break; // Unreachable
    }

    // Pending load
    load_reg = rt;
    load_value = value;
}

void CPU::SB(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store8 while cache is isolated\n");
        return;
    }

    store<uint8_t>(get_reg(rs) + imm16_se, (uint8_t) get_reg(rt));
}

void CPU::SH(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store16 while cache is isolated\n");
        return;
    }

    uint32_t address = get_reg(rs) + imm16_se;
    if (address % 2 != 0) {
        exception(EXCEPTION_STORE_ADDRESS_ERROR);
    } else {
        store<uint16_t>(address, (uint16_t) get_reg(rt));
    }
}

void CPU::SWL(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;
    uint32_t value = get_reg(rt);

    uint32_t aligned_address = address & ~0x00000003; // Clear two last bits
    uint32_t memory = load<uint32_t>(aligned_address);
    switch(address & 0x03) {
    case 0: memory = (memory & 0xFFFFFF00) | (value >> 24); break;
    case 1: memory = (memory & 0xFFFF0000) | (value >> 16); break;
    case 2: memory = (memory & 0xFF000000) | (value >> 8); break;
    case 3: memory = (memory & 0x00000000) | (value >> 0); break;
    default: exit(1); break; // Unreachable
    }

    store<uint32_t>(address, memory);
}

void CPU::SW(size_t rs, size_t rt, int32_t imm16_se)
{
    if (SR & SR_CACHE_ISOLATION) {
        //debug("Ignoring store32 while cache is isolated\n");
        return;
    }

    uint32_t address = get_reg(rs) + imm16_se;
    if (address % 4 != 0) {
        exception(EXCEPTION_STORE_ADDRESS_ERROR);
    } else {
        store<uint32_t>(address, get_reg(rt));
    }
}

void CPU::SWR(size_t rs, size_t rt, int32_t imm16_se)
{
    uint32_t address = get_reg(rs) + imm16_se;
    uint32_t value = get_reg(rt);

    uint32_t aligned_address = address & ~0x00000003; // Clear two last bits
    uint32_t memory = load<uint32_t>(aligned_address);
    switch(address & 0x03) {
    case 0: memory = (memory & 0x00000000) | (value << 0); break;
    case 1: memory = (memory & 0x000000FF) | (value << 8); break;
    case 2: memory = (memory & 0x0000FFFF) | (value << 16); break;
    case 3: memory = (memory & 0x00FFFFFF) | (value << 24); break;
    default: exit(1); break; // Unreachable
    }

    store<uint32_t>(address, memory);
}

void CPU::LWC0()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::LWC1()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::LWC2(uint32_t data)
{
    error("Unhandled GTE LWC 0x%08x\n", data);
    exit(1);
}

void CPU::LWC3()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::SWC0()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::SWC1()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
}

void CPU::SWC2(uint32_t data)
{
    error("Unhandled GTE SWC 0x%08x\n", data);
    exit(1);
}

void CPU::SWC3()
{
    exception(EXCEPTION_COPROCESSOR_ERROR);
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

void CPU::SLLV(size_t rs, size_t rt, size_t rd)
{
    // Truncate shift amount to 5 bits
    set_reg(rd, get_reg_se(rt) << (get_reg(rs) & MASK_5_BITS));
}

void CPU::SRLV(size_t rs, size_t rt, size_t rd)
{
    // Truncate shift amount to 5 bits
    set_reg(rd, get_reg(rt) >> (get_reg(rs) & MASK_5_BITS));
}

void CPU::SRAV(size_t rs, size_t rt, size_t rd)
{
    // Truncate shift amount to 5 bits
    set_reg(rd, get_reg_se(rt) >> (get_reg(rs) & MASK_5_BITS));
}

void CPU::JR(size_t rs)
{
    isBranch = true;
    nextPC = get_reg(rs);
}

void CPU::JALR(size_t rs, size_t rd)
{
    set_reg(rd, nextPC);

    JR(rs);
}

void CPU::SYSCALL()
{
    exception(EXCEPTION_SYSCALL);
}

void CPU::BREAK()
{
    exception(EXCEPTION_BREAK);
}

void CPU::MFHI(size_t rd)
{
    set_reg(rd, HI);
}

void CPU::MTHI(size_t rs)
{
    HI = get_reg(rs);
}

void CPU::MFLO(size_t rd)
{
    set_reg(rd, LO);
}

void CPU::MTLO(size_t rs)
{
    LO = get_reg(rs);
}

void CPU::MULT(size_t rs, size_t rt)
{
    int64_t a = (int64_t) get_reg_se(rs);
    int64_t b = (int64_t) get_reg_se(rt);

    uint64_t v = (uint64_t) (a * b);

    HI = (uint32_t) (v >> 32);
    LO = (uint32_t) v;
}

void CPU::MULTU(size_t rs, size_t rt)
{
    uint64_t a = (uint64_t) get_reg(rs);
    uint64_t b = (uint64_t) get_reg(rt);

    uint64_t v = a * b;

    HI = (uint32_t) (v >> 32);
    LO = (uint32_t) v;
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
        exception(EXCEPTION_OVERFLOW);
    }

    set_reg(rd, (uint32_t) result);
}

void CPU::ADDU(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) + get_reg(rt));
}

void CPU::SUB(size_t rs, size_t rt, size_t rd)
{
    int64_t s = (int64_t) get_reg_se(rs);
    int64_t t = (int64_t) get_reg_se(rt);

    if ((s < 0 && t > (s - (int64_t)INT32_MIN)) ||
        (s > 0 && t < -((int64_t)INT32_MAX - s))) {
        exception(EXCEPTION_OVERFLOW);
    }

    int64_t result = s - t;

    set_reg(rd, (uint32_t) result);
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

void CPU::XOR(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, get_reg(rs) ^ get_reg(rt));
}

void CPU::NOR(size_t rs, size_t rt, size_t rd)
{
    set_reg(rd, !(get_reg(rs) | get_reg(rt)));
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
    case 12: load_value = SR; break;
    case 13: load_value = CAUSE; break;
    case 14: load_value = EPC; break;
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

void CPU::RFE()
{
    // Restore the pre-exception mode
    uint32_t mode = SR & MASK_6_BITS;  // Store last 6 bits
    SR &= ~MASK_6_BITS;                // Clear last 6 bits
    SR |= (mode >> 2);                 // Shift the stack to the right
}
