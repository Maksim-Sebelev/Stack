#include <malloc.h>
#include "StackFunc.h"
#include "ErrorFunc.h"
#include "HashFunc.h"

//----------------------------------------------------------------------------------------------------------------------

ErrorType Ctor(Stack_t* Stack, size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name))
{
    ErrorType Err = {};

    Stack->Capacity = (StackDataSize > MinCapacity) ? StackDataSize + 2 : MinCapacity + 2;
    Stack->Data = (StackElem_t*) calloc (Stack->Capacity, sizeof(StackElem_t));
    ON_DEBUG
    (
    if (Stack->Data == NULL)
    {
        Err.FatalError.DataNull = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }
    );
    Stack->Size = 1;
    Stack->LeftStackCanary  = LeftStackCanary;
    Stack->RightStackCanary = RightStackCanary;
    Stack->Data[0] = LeftDataCanary;
    Stack->Data[Stack->Capacity - 1] = RightDataCanary;

    for (size_t Data_i = 1; Data_i < Stack->Capacity - 1; Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    }
    ON_DEBUG
    (
    Stack->Var.File  = File;
    Stack->Var.Line  = Line;
    Stack->Var.Func  = Func;
    Stack->Var.Name  = Name;
    Stack->DataHash  = Hash(Stack->Data, Stack->Capacity * sizeof(StackElem_t));
    Stack->StackHash = CalcRealStackHash(Stack);
    );

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

    Stack->Size++;

    if (Stack->Size > MaxCapacity)
    {
        Err.Warning.PushInFullStack = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    if (Stack->Size - 1 <= Stack->Capacity - 2)
    {
        Stack->Data[Stack->Size - 1] = PushElem;
        ON_DEBUG
        (
        Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
        Stack->StackHash = CalcRealStackHash(Stack);
        );

        RETURN(Stack, Err);
    }

    
    Stack->Capacity = ((Stack->Capacity - 2) * CapPushReallocCoef + 2) < (MaxCapacity + 2) ? ((Stack->Capacity - 2) * CapPushReallocCoef + 2) : (MaxCapacity + 2);
    ON_DEBUG(Stack->StackHash = CalcRealStackHash(Stack));

    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t));
    
    ON_DEBUG
    (
    if (Stack->Data == NULL)
    {
        Err.FatalError.ReallocPushNull = 1;
        Err.IsFatalError = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }
    );

    Stack->Data[Stack->Size - 1] = PushElem;
    Stack->Data[Stack->Capacity - 1] = RightDataCanary;

    if (Stack->Capacity == MaxCapacity + 2)
    {
        Err.Warning.ToBigCapacity = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    ON_DEBUG
    (
    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity - 1; Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    } 

    Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);
    );

    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType Pop(Stack_t* Stack, StackElem_t* PopElem)
{
    ErrorType Err = {};
    ERR_RETURN_WARN_PRINT(Stack, Err);

    if (Stack->Size == 1)
    {
        Err.Warning.PopInEmptyStack = 1;
        Err.IsWarning = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err);
    }

    Stack->Size--;
    *PopElem = Stack->Data[Stack->Size + 1];

    ON_DEBUG
    (
    Stack->Data[Stack->Size] = Poison;
    Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);
    );

    if ((Stack->Size - 1) * CapPopReallocCoef > (Stack->Capacity - 2))
    {
        RETURN(Stack, Err);
    }

    Stack->Capacity = (((Stack->Capacity - 2) / CapPopReallocCoef) + 2 > MinCapacity + 2) ? (((Stack->Capacity - 2) / CapPopReallocCoef) + 2) : (MinCapacity + 2);
    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t)); //тогда предполагается что между ctor и dtor capacity > 0

    ON_DEBUG
    (
    if (Stack->Data == NULL)
    {
        Err.FatalError.ReallocPopNull = 1;
        Err.IsFatalError = 1;
        ERR_RETURN_WARN_PRINT(Stack, Err); // log 
    }
    );

    Stack->Data[Stack->Capacity - 1] = RightDataCanary;

    ON_DEBUG
    (
    Stack->DataHash  = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);
    );

    RETURN(Stack, Err);
}

//----------------------------------------------------------------------------------------------------------------------

ErrorType PrintStack(Stack_t* Stack)
{
    ErrorType Err = {};

    printf("\nStack:\n");
    for (size_t Stack_i = 1; Stack_i < Stack->Size; Stack_i++)
    {
        printf("%d ", Stack->Data[Stack_i]);
    }
    printf("\nStack end\n\n");

    Verif(Stack, &Err);
    return Err;
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

ON_CANARY
(
Canary_t GetLeftDataCanary(Stack_t* Stack)
{
    return Stack->Data[0];
}

Canary_t GetRightDataCanary(Stack_t* Stack)
{
    return Stack->Data[Stack->Capacity - 1];
}
);

