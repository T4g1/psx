#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>

#include "log.h"
#include "psx.h"

#include "main.h"


void show_usage()
{
    std::cerr << "Usage: psx <option(s)> ROM\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-b,--boot BOOT\tSpecifies BOOT ROM\n";
}


int main(int argc, char *argv[])
{
    info("PSX emulation\n");

    if (argc < 2 || argc > 5) {
        show_usage();
    }

    std::string rom;
    std::string boot = "";
    std::string palette = "0";

    for (int i=1; i<argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage();
            return EXIT_SUCCESS;
        } else if ((arg == "-b") || (arg == "--boot")) {
            if (i + 1 < argc) {
                boot = argv[++i];
            } else {
                error("--boot option requires one argument\n");
                show_usage();
                return EXIT_FAILURE;
            }
        } else if (i == argc - 1) {
            rom = argv[i];
        } else {
            error("Option not recognized: %s\n", argv[i]);
            show_usage();
            return EXIT_FAILURE;
        }
    }

    PSX *psx = new PSX();
    if (!psx->init(boot.c_str(), rom.c_str())) {
        return EXIT_FAILURE;
    }

    int status = psx->run();

    delete psx;

    return status;
}
