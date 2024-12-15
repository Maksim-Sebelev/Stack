#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include "Hash.hpp"


uint64_t Hash(void* Arr, size_t ArrElemQuant, size_t ArrElemSize)
{
    assert(Arr != NULL);

    char* ArrChar = (char*) Arr;

    static const uint64_t RandomBigNum = 91274932787238748;
    uint64_t ArrHash = RandomBigNum;

    for (size_t Arr_i = 0; Arr_i < ArrElemQuant * ArrElemSize; Arr_i++)
    { 
        assert((ArrChar + Arr_i) != NULL);
        ArrHash = (ArrHash * 33) ^ ArrChar[Arr_i];
    }

    return ArrHash;
}
