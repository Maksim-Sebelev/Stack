#include <malloc.h>
#include <assert.h>
#include "HashFunc.h"
#include "Stack.h"

//----------------------------------------------------------------------------------------------------------------------

int Hash(void* Arr, size_t ArrSize)
{
    ON_DEBUG(assert(Arr != NULL));
    ON_DEBUG(assert(ArrSize >= 0));

    char* ArrCharPtr = (char*) Arr;
    int ArrHash = 5381;

    for (size_t Arr_i = 0; Arr_i < ArrSize; Arr_i++)
    {
        ON_DEBUG(assert((ArrCharPtr + Arr_i) != NULL));
        ArrHash = (ArrHash * 31) ^ ArrCharPtr[Arr_i];
    }

    return ArrHash;
}

//----------------------------------------------------------------------------------------------------------------------

ON_DEBUG
(
int CalcRealStackHash(Stack_t* Stack)
{
    int StackHashCopy = Stack->StackHash;
    Stack->StackHash = 0;
    int RealStackHash = Hash((void*)Stack, sizeof(*Stack));
    Stack->StackHash = StackHashCopy;
    return RealStackHash;
}
);

//----------------------------------------------------------------------------------------------------------------------

