#include <malloc.h>
#include "StackFunc.h"
#include "ErrorFunc.h"
#include "HashFunc.h"
#include "Stack.h"

//----------------------------------------------------------------------------------------------------------------------

ErrorType Ctor(Stack_t* Stack, size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name))
{
    ErrorType Err = {};

    Stack->Capacity = GetNewCtorCapacity(StackDataSize);
    CtorCallocDataWithNewCapacity(Stack, &Err);

    // ERR_RETURN_WARN_PRINT(Stack, Err);

    Stack->Size = 0;
    ON_SCANARY
    (
    Stack->LeftStackCanary  = LeftStackCanary;
    Stack->RightStackCanary = RightStackCanary;
    )

    ON_DCANARY
    (
    Stack->Data[GetLeftDataCanaryIndex(Stack)]  = LeftDataCanary;
    Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;
    )

    ON_POISON
    (
    for (size_t Data_i = 0; Data_i < Stack->Capacity; Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    }
    )

    ON_DEBUG
    (
    Stack->Var.File = File;
    Stack->Var.Line = Line;
    Stack->Var.Func = Func;
    Stack->Var.Name = Name;
    )

    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, Stack->Capacity * sizeof(StackElem_t));)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

    // DUMP(Stack);

    return VERIF(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Dtor(Stack_t* Stack)
{
    ErrorType Err = {};
    DtorFreeData(Stack, &Err);
    Stack->Data = NULL;
    Stack->Capacity = 0;
    Stack->Size = 0;
    return Err;
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Push(Stack_t*  Stack, StackElem_t PushElem)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    if (Stack->Size + 1 > MaxCapacity)
    {
        Err.Warning.PushInFullStack = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    Stack->Size++;

    if (Stack->Size <= Stack->Capacity)
    {
        Stack->Data[Stack->Size - 1] = PushElem;
    
        ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
        ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

        return VERIF(Stack, Err);
    }

    PushReallocWithNewCapacity(Stack, &Err);

    if (Err.IsFatalError == 1)
    {
        return Err;
    }

    Stack->Data[Stack->Size - 1] = PushElem;
    ON_DCANARY(Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;)

    ON_POISON
    (
    for (size_t Data_i = Stack->Size; Data_i < GetRightDataCanaryIndex(Stack); Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    }
    )

    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)
    
    if (Stack->Capacity == MaxCapacity)
    {
        Err.Warning.ToBigCapacity = 1;
        Err.IsWarning = 1;
    }
    return VERIF(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Pop(Stack_t* Stack, StackElem_t* PopElem)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    if (Stack->Size == 0)
    {
        Err.Warning.PopInEmptyStack = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    *PopElem = Stack->Data[Stack->Size];
    Stack->Size--;

    ON_POISON(Stack->Data[Stack->Size] = Poison;)
    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

    if (Stack->Size * CapPopReallocCoef > Stack->Capacity)
    {
        return VERIF(Stack, Err);
    }

    PopReallocWithNewCapacity(Stack, &Err);

    if (Err.IsFatalError == 1)
    {
        return Err;
    }

    ON_DCANARY(Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;)
    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

    return VERIF(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType PrintStack(Stack_t* Stack)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    printf("\nStack:\n");
    for (size_t Stack_i = 0; Stack_i < Stack->Size; Stack_i++)
    {
        printf("%d ", Stack->Data[Stack_i]);
    }
    printf("\nStack end\n\n");

    return VERIF(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ON_SHASH
(
uint64_t CalcRealStackHash(Stack_t* Stack)
{
    uint64_t StackHashCopy = Stack->StackHash;
    Stack->StackHash = 0;
    uint64_t RealStackHash = Hash(Stack, sizeof(*Stack));
    Stack->StackHash = StackHashCopy;
    return RealStackHash;
}
);

//----------------------------------------------------------------------------------------------------------------------

ON_DCANARY
(
DataCanary_t GetLeftDataCanary(Stack_t* Stack)
{
    return Stack->Data[GetLeftDataCanaryIndex(Stack)];
}

DataCanary_t GetRightDataCanary(Stack_t* Stack)
{
    return Stack->Data[GetRightDataCanaryIndex(Stack)];
}

size_t GetLeftDataCanaryIndex(Stack_t* Stack)
{
    return -1;
}
);

size_t GetRightDataCanaryIndex(Stack_t* Stack)
{
    return Stack->Capacity;
}


size_t GetNewCtorCapacity(size_t StackDataSize)
{
    size_t temp = (StackDataSize > MinCapacity) ? StackDataSize : MinCapacity;
    return GetNewCapacity(temp);
}

size_t GetNewPushCapacity(Stack_t* Stack)
{
    size_t NewCapacity = (Stack->Capacity * CapPushReallocCoef);
    return GetNewCapacity(NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity);
}

size_t GetNewPopCapacity(Stack_t* Stack)
{
    size_t NewCapacity = (Stack->Capacity / CapPopReallocCoef);
    size_t temp = (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
    return GetNewCapacity(temp);
}

size_t GetNewCapacity(size_t Capacity)
{
    ON_DCANARY
    (
    while (Capacity % sizeof(DataCanary_t) != 0)
    {
        Capacity++;
    }
    )
    return Capacity;
}

ErrorType PushReallocWithNewCapacity(Stack_t* Stack, ErrorType* Err)
{
    Stack->Capacity = GetNewPushCapacity(Stack);
    ON_DCANARY
    (
    Stack->Data--;
    Stack->Data--;
    )
    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t) ON_DCANARY(+ 2 * sizeof(DataCanary_t)));
    if (Stack->Data == NULL)
    {
        Err->FatalError.ReallocPushNull = 1;
        Err->IsFatalError = 1;
        return *Err;
    }
    ON_DCANARY
    (
    Stack->Data++;
    Stack->Data++;
    )
    return *Err; 
}


ErrorType PopReallocWithNewCapacity(Stack_t* Stack, ErrorType* Err)
{
    Stack->Capacity = GetNewPopCapacity(Stack);
    ON_DCANARY
    (
    Stack->Data--;
    Stack->Data--;
    )
    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t) ON_DCANARY(+ 2 * sizeof(DataCanary_t)));
    if (Stack->Data == NULL)
    {
        Err->FatalError.ReallocPopNull = 1;
        Err->IsFatalError = 1;
        return *Err;
    }
    ON_DCANARY
    (
    Stack->Data++;
    Stack->Data++;
    )
    return *Err;
}

ErrorType CtorCallocDataWithNewCapacity(Stack_t* Stack, ErrorType* Err)
{
    Stack->Data = (StackElem_t*) calloc (Stack->Capacity * sizeof(StackElem_t) ON_DCANARY(+ 2 * sizeof(DataCanary_t)), sizeof(char));
    if (Stack->Data == NULL)
    {
        Err->FatalError.DataNull = 1;
        Err->IsFatalError = 1;
        return *Err;
    }
    ON_DCANARY
    (
    Stack->Data++;
    Stack->Data++;
    )
    return *Err;
}

ErrorType DtorFreeData(Stack_t* Stack, ErrorType* Err)
{
    ON_DCANARY
    (
    Stack->Data--;
    Stack->Data--;
    )
    free(Stack->Data);
    return *Err;
}
