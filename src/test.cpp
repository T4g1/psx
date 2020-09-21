#include "test.h"

#include <initializer_list>

#include "log.h"
#include "psx.h"


bool test_placeholder()
{
    return true;
}


int main(void)
{
    fprintf(stdout, "PSX auto testing\n");

    test("Placeholder: Fake test", &test_placeholder);

    return EXIT_SUCCESS;
}
