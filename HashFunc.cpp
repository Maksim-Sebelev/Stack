#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "HashFunc.h"

uint64_t Hash(void* Arr, size_t ArrElemQuant, size_t ArrElemSize)
{
    assert(Arr != NULL);

    char* ArrChar = (char*) Arr;
    uint64_t ArrHash = 5381;

    for (size_t Arr_i = 0; Arr_i < ArrElemQuant * ArrElemSize; Arr_i++)
    {
        assert((ArrChar + Arr_i) != NULL);
        ArrHash = (ArrHash * 33) ^ ArrChar[Arr_i];
    }

    return ArrHash;
}
