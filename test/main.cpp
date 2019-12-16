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

    return 0;
}
