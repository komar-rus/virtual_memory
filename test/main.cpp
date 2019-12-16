#include <iostream>
#include "../src/virtual_memory.h"

#include <utility>

using namespace std;

void vmfill(virtual_memory &vm)
{
    for (int a = 0; a < 10; ++a)
    {
        size_t size = 100;
        uint8_t *pData = new uint8_t[size];
        std::memset(pData, (uint8_t)a, size);
        vm.push_back(pData, 100);
    }
}

int main()
{
    virtual_memory vm;
    vmfill(vm);

    uint8_t a = vm[0];
    vm += 100;

    uint8_t b = vm[0];
    vm += 100;

    uint8_t c = vm[0];
    vm += 100;

    if (0 != a
        && 1 != b
        && 2 != c)
    {
        return -1;
    }

    virtual_memory vm1;
    vmfill(vm1);

    uint8_t a1 = vm1[0];

    vm1 += 100;
    vm1 -= 100;

    uint8_t b2 = vm1[0];

    if (a1 != b2)
    {
        return -1;
    }

    return 0;
}
