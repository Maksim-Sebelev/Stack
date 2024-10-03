#include <malloc.h>
#include "StackFunc.h"
#include "ErrorFunc.h"
#include "HashFunc.h"
#include "Stack.h"

//----------------------------------------------------------------------------------------------------------------------

ErrorType Ctor(Stack_t* Stack, size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name))
{
    ErrorType Err = {};

    Stack->Capacity = (StackDataSize ON_DCANARY(+ 2) > MinCapacity) ? StackDataSize ON_DCANARY(+ 2) : MinCapacity;
    Stack->Data = (StackElem_t*) calloc (Stack->Capacity, sizeof(StackElem_t));
    if (Stack->Data == NULL)
    {
        Err.FatalError.DataNull = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    Stack->Size = GetDataBeginIndex(Stack);
    ON_SCANARY
    (
    Stack->LeftStackCanary  = LeftStackCanary;
    Stack->RightStackCanary = RightStackCanary;
    )

    ON_DCANARY
    (
    Stack->Data[GetLeftDataCanaryIndex(Stack)] = LeftDataCanary;
    Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;
    )

    ON_POISON
    (
    for (size_t Data_i = 1; Data_i < Stack->Capacity - 1; Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    }
    )

    ON_DEBUG
    (
    Stack->Var.File  = File;
    Stack->Var.Line  = Line;
    Stack->Var.Func  = Func;
    Stack->Var.Name  = Name;
    )

    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, Stack->Capacity * sizeof(StackElem_t));)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)
    // DUMP(Stack);
    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------


ErrorType Dtor(Stack_t* Stack)
{
    free(Stack->Data);
    Stack->Data = NULL;
    Stack->Capacity = 0;
    Stack->Size = 0;
    ErrorType Err = {};
    return Err;
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Push(Stack_t*  Stack, StackElem_t PushElem)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    if (GetStackSize(Stack) + 1 > GetMaxCapacity())
    {
        Err.Warning.PushInFullStack = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    Stack->Size++;

    if (GetStackSize(Stack) <= GetStackCapacity(Stack))
    {
        Stack->Data[Stack->Size - 1] = PushElem;
    
        ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
        ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

        RETURN(Stack, Err);
    }

    
    Stack->Capacity = GetNewPushCapacity(Stack);
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack));

    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t));

    if (Stack->Data == NULL)
    {
        Err.FatalError.ReallocPushNull = 1;
        Err.IsFatalError = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    Stack->Data[Stack->Size - 1] = PushElem;
    ON_DCANARY(Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;)

    ON_POISON
    (
    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity ON_DCANARY(- 1); Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    } 
    );

    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)
    
    if (Stack->Capacity == MaxCapacity)
    {
        Err.Warning.ToBigCapacity = 1;
        Err.IsWarning = 1;
    }
    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Pop(Stack_t* Stack, StackElem_t* PopElem)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    if (Stack->Size == GetDataBeginIndex(Stack))
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

    if (GetStackSize(Stack) * CapPopReallocCoef > GetStackCapacity(Stack))
    {
        RETURN(Stack, Err);
    }

    Stack->Capacity = GetNewPopCapacity(Stack);

    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t)); //тогда предполагается что между ctor и dtor capacity > 0

    if (Stack->Data == NULL)
    {
        Err.FatalError.ReallocPopNull = 1;
        Err.IsFatalError = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err); // log 
    }

    ON_DCANARY(Stack->Data[GetRightDataCanaryIndex(Stack)] = RightDataCanary;)
    ON_DHASH(Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);)
    ON_SHASH(Stack->StackHash = CalcRealStackHash(Stack);)

    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType PrintStack(Stack_t* Stack)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    printf("\nStack:\n");
    for (size_t Stack_i = GetDataBeginIndex(Stack); Stack_i < Stack->Size; Stack_i++)
    {
        printf("%d ", Stack->Data[Stack_i]);
    }
    printf("\nStack end\n\n");

    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ON_SHASH
(
int CalcRealStackHash(Stack_t* Stack)
{
    int StackHashCopy = Stack->StackHash;
    Stack->StackHash  = 0;
    int RealStackHash = Hash((void*)Stack, sizeof(*Stack));
    Stack->StackHash  = StackHashCopy;
    return RealStackHash;
}
);

//----------------------------------------------------------------------------------------------------------------------

ON_DCANARY
(
DataCanary_t GetLeftDataCanary(Stack_t* Stack)
{
    return Stack->Data[0];
}

DataCanary_t GetRightDataCanary(Stack_t* Stack)
{
    return Stack->Data[Stack->Capacity - 1];
}

size_t GetLeftDataCanaryIndex(Stack_t* Stack)
{
    return 0;
}

size_t GetRightDataCanaryIndex(Stack_t* Stack)
{
    return Stack->Capacity - 1;
}
);


size_t GetStackCapacity(Stack_t* Stack)
{
    return Stack->Capacity ON_DCANARY(- 2);
}

size_t GetStackSize(Stack_t* Stack)
{
    return Stack->Size ON_DCANARY(- 1);
}



size_t GetNewPushCapacity(Stack_t* Stack)
{
    size_t NewCapacity = (GetStackCapacity(Stack) * CapPushReallocCoef) ON_DCANARY(+ 2);
    return NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity;
}

size_t GetNewPopCapacity(Stack_t* Stack)
{
    size_t NewCapacity = (GetStackCapacity(Stack) / CapPopReallocCoef) ON_DCANARY(+ 2);
    return (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
}

size_t GetDataBeginIndex(Stack_t* Stack)
{
    return 0 ON_DCANARY(+ 1);
}

size_t GetMaxCapacity()
{
    return MaxCapacity ON_DCANARY(- 2);
}


size_t GetMinCapacity()
{
    return MinCapacity ON_DCANARY(- 2);
}


