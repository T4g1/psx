#include "tools.h"

#include <iostream>

#include "log.h"
#include "instruction.h"


void show_usage()
{
    std::cerr << "Convert a hex string into assembly human readable\n";
    std::cerr << "Usage: tools [hex string]\n";
}


int main(int argc, char *argv[])
{
    info("PSX tools\n");

    if (argc != 2) {
        show_usage();

        return EXIT_FAILURE;
    }

    const char *hexstring = argv[1];
    unsigned long number = strtoul(hexstring, NULL, 16);

    decode((uint32_t) number);

    return EXIT_SUCCESS;
}
