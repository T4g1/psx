#include "test.h"

#include <iostream>
#include <initializer_list>

#include "log.h"
#include "cpu.h"
#include "bios.h"
#include "interconnect.h"


std::string bios_path = "";
CPU *cpu;
BIOS *bios;
Interconnect *inter;


void show_usage()
{
    std::cerr << "Usage: test [BIOS]\n";
}


bool test_init()
{
    cpu = new CPU();
    bios = new BIOS();
    inter = new Interconnect();

    bool running = true;
    running &= cpu->init();
    running &= bios->init(bios_path);
    running &= inter->init(bios);

    if (running) {
        cpu->set_inter(inter);
    }

    ASSERT(running);

    return running;
}


bool test_ORI()
{
    cpu->reset();
    cpu->set_reg(2, 0x0000FFFF);
    cpu->set_reg(3, 0xFFFFFFFF);

    cpu->ORI(0, 1, 0x0000DEAD);
    cpu->ORI(2, 2, 0x0000DEAD);
    cpu->ORI(3, 3, 0x0000DEAD);

    ASSERT(cpu->get_reg(1) == 0x0000DEAD);
    ASSERT(cpu->get_reg(2) == 0x0000FFFF);
    ASSERT(cpu->get_reg(3) == 0xFFFFFFFF);

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

    test("CPU: ORI", &test_ORI);

    return EXIT_SUCCESS;
}
