#include "test.h"

#include <iostream>
#include <initializer_list>

#include "instruction.h"
#include "log.h"
#include "cpu.h"
#include "spu.h"
#include "bios.h"
#include "ram.h"
#include "interconnect.h"


std::string bios_path = "";
CPU *cpu;
SPU *spu;
BIOS *bios;
RAM *ram;
Interconnect *inter;


void show_usage()
{
    std::cerr << "Usage: test [BIOS]\n";
}


bool test_init()
{
    cpu = new CPU();
    spu = new SPU();
    bios = new BIOS();
    ram = new RAM();
    inter = new Interconnect();

    bool running = true;
    running &= cpu->init();
    running &= spu->init();
    running &= bios->init(bios_path);
    running &= ram->init();
    running &= inter->init(spu, bios, ram);

    if (running) {
        cpu->set_inter(inter);
    }

    ASSERT(running);

    return running;
}


bool test_instruction()
{
    ASSERT(get_imm16(0x00000000) == 0x0000);
    ASSERT(get_imm16(0x00000001) == 0x0001);
    ASSERT(get_imm16(0x000001AD) == 0x01AD);
    ASSERT(get_imm16(0x0000FFFF) == 0xFFFF);
    ASSERT(get_imm16(0x000083C5) == 0x83C5);

    ASSERT(get_imm16_se(0x00000000) == (int32_t) 0x00000000);
    ASSERT(get_imm16_se(0x00000001) == (int32_t) 0x00000001);
    ASSERT(get_imm16_se(0x000001AD) == (int32_t) 0x000001AD);
    ASSERT(get_imm16_se(0x0000FFFF) == (int32_t) 0xFFFFFFFF);
    ASSERT(get_imm16_se(0x000083C5) == (int32_t) 0xFFFF83C5);

    return true;
}


bool test_signed()
{
    uint16_t uint_a = 0xFFFF;
    int16_t int_a = 0xFFFF;

    ASSERT(uint_a == 65535);
    ASSERT(int_a == -1);

    ASSERT((int16_t) uint_a == -1);
    ASSERT((int32_t)(int16_t) uint_a == -1);

    ASSERT((int32_t) int_a == -1);
    ASSERTV((int32_t) int_a == (int32_t) 0xFFFFFFFF, "0x%08x", (int32_t) int_a);

    return true;
}


/*********************************
 * CPU OPCODES
 *********************************/

bool test_ADDI()
{
    cpu->reset();

    // Overflow
    //cpu->set_reg(1, 0xFFFFFFFF);
    //cpu->ADDI(1, 1, 0x00000001);
    //ASSERT(cpu->get_reg(1) == 0x00000000);

    // Underflow
    cpu->force_set_reg(1, 0x00000000);
    cpu->ADDI(1, 1, 0xFFFFFFFF); // 0 - 1
    ASSERTV(cpu->force_get_reg(1) == 0xFFFFFFFF, "Got 0x%08x\n", cpu->force_get_reg(1));

    cpu->force_set_reg(1, 0x00000020);
    cpu->ADDI(1, 1, 0xFFFFFFFC); // 32 - 3
    ASSERTV(cpu->force_get_reg(1) == 0x0000001C, "Got 0x%08x\n", cpu->force_get_reg(1));

    return true;
}

bool test_ADDIU()
{
    cpu->reset();
    cpu->force_set_reg(1, 0xFFFFFFFF);

    cpu->ADDIU(1, 1, (int32_t)0x00000001);
    ASSERT(cpu->force_get_reg(1) == 0x00000000);

    cpu->ADDIU(1, 1, 0xFFFFFFFF);
    ASSERT(cpu->force_get_reg(1) == 0xFFFFFFFF);

    return true;
}

bool test_ORI()
{
    cpu->reset();
    cpu->force_set_reg(2, 0x0000FFFF);
    cpu->force_set_reg(3, 0xFFFFFFFF);

    cpu->ORI(0, 1, 0x0000DEAD);
    cpu->ORI(2, 2, 0x0000DEAD);
    cpu->ORI(3, 3, 0x0000DEAD);

    ASSERT(cpu->force_get_reg(1) == 0x0000DEAD);
    ASSERT(cpu->force_get_reg(2) == 0x0000FFFF);
    ASSERT(cpu->force_get_reg(3) == 0xFFFFFFFF);

    return true;
}


int main(int argc, char *argv[])
{
    info("PSX testing\n");

    if (argc != 2) {
        show_usage();

        return EXIT_FAILURE;
    }

    bios_path = argv[1];
    if (bios_path.empty()) {
        error("You need to specify a BIOS file to use\n");
        show_usage();

        return EXIT_FAILURE;
    }

    test("Generic: Initialisation", &test_init);
    test("Generic: Instruction", &test_instruction);
    test("Generic: Signed numbers", &test_signed);

    test("CPU: ADDI", &test_ADDI);
    test("CPU: ADDIU", &test_ADDIU);
    test("CPU: ORI", &test_ORI);

    return EXIT_SUCCESS;
}
