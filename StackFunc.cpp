#include <malloc.h>
#include "StackFunc.h"
#include "ErrorFunc.h"
#include "HashFunc.h"

//----------------------------------------------------------------------------------------------------------------------

ErrorCode Ctor(Stack_t* Stack, size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name))
{
    Stack->Capacity = (StackDataSize > MinCapacity) ? StackDataSize ON_DEBUG(+ 2): MinCapacity ON_DEBUG(+ 2);
    Stack->Data = (StackElem_t*) calloc (Stack->Capacity, sizeof(StackElem_t));
    Stack->Size = 0 ON_DEBUG(+ 1);

    if (Stack->Data == NULL)
    {
        return CALLOC_CTOR_NULL;
    }

    ON_DEBUG
    (
    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity; Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    }
    Stack->LeftStackCanary = LeftStackCanary;
    Stack->RightStackCanary = RightStackCanary;
    Stack->Data[0] = LeftDataCanary;
    Stack->Data[Stack->Capacity - 1] = RightDataCanary; 
    Stack->Var.File = File;
    Stack->Var.Line = Line;
    Stack->Var.Func = Func;
    Stack->Var.Name = Name;
    Stack->DataHash = Hash(Stack->Data, Stack->Capacity * sizeof(StackElem_t));
    Stack->StackHash = CalcRealStackHash(Stack);
    return Verif(Stack);
    );

    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------


ErrorCode Dtor(Stack_t* Stack)
{
    free(Stack->Data);
    Stack->Data = NULL;
    Stack->Capacity = 0;
    Stack->Size = 0;
    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

ErrorCode Push(Stack_t* Stack, StackElem_t PushElem)
{
    ON_DEBUG(ERROR_RETURN(Verif(Stack)));

    Stack->Size++;

    if (Stack->Size ON_DEBUG(- 1) <= Stack->Capacity ON_DEBUG(- 2))
    {
        Stack->Data[Stack->Size - 1] = PushElem;
        ON_DEBUG
        (
        Stack->DataHash = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
        Stack->StackHash = CalcRealStackHash(Stack);
        return Verif(Stack);
        );
        return NO_ERR;
    }

    ON_DEBUG
    (
        if (Stack->Size ON_DEBUG(- 1) > MaxCapacity + ON_DEBUG(+ 2))
        {
            return PUSH_IN_FULL_STACK;
        }
    );
    
    Stack->Capacity = ((Stack->Capacity ON_DEBUG(- 2)) * CapPushReallocCoef ON_DEBUG(+ 2) < MaxCapacity) ? (Stack->Capacity ON_DEBUG(- 2)) * CapPushReallocCoef ON_DEBUG(+ 2): MaxCapacity;
    ON_DEBUG(Stack->StackHash = CalcRealStackHash(Stack));

    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t));
    
    ON_DEBUG
    (
        if (Stack->Data == NULL)
        {
            return REALLOC_PUSH_NULL;
        }
    );

    Stack->Data[Stack->Size - 1] = PushElem;

    ON_DEBUG
    (
    Stack->Data[Stack->Capacity - 1] = RightDataCanary;
    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity ON_DEBUG(- 1); Data_i++)
    {
        Stack->Data[Data_i] = Poison;
    } 

    Stack->DataHash = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);

    ERROR_RETURN(Verif(Stack));

    if (Stack->Capacity == MaxCapacity)
    {
        return TO_BIG_CAPACITY;
    }

    return Verif(Stack);
    );

    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

ErrorCode Pop(Stack_t* Stack, StackElem_t* PopElem)
{

    ON_DEBUG
    (
    ERROR_RETURN(Verif(Stack));

    if (Stack->Size == 0 ON_DEBUG(+ 1))
    {
        return POP_IN_EMPTY_STACK;
    }
    );

    Stack->Size--;
    *PopElem = Stack->Data[Stack->Size ON_DEBUG(+ 1)];
    ON_DEBUG
    (
    Stack->Data[Stack->Size] = Poison;
    Stack->DataHash = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);

    if (Stack->Size ON_DEBUG(- 1) > (Stack->Capacity ON_DEBUG(- 2)) / CapPopReallocCoef)
    {
        return Verif(Stack);
    }
    );


    if (Stack->Size ON_DEBUG(- 1) > (Stack->Capacity ON_DEBUG(- 2)) / CapPopReallocCoef)
    {
        return NO_ERR;
    }

    Stack->Capacity = (((Stack->Capacity ON_DEBUG(- 2)) / CapPopReallocCoef) ON_DEBUG(+ 2) > MinCapacity ON_DEBUG(+ 2)) ? (((Stack->Capacity ON_DEBUG(- 2)) / CapPopReallocCoef) ON_DEBUG(+ 2)) : (MinCapacity ON_DEBUG(+ 2));
    Stack->Data = (StackElem_t*) realloc(Stack->Data, Stack->Capacity * sizeof(StackElem_t)); //тогда предполагается что между ctor и dtor capacity > 0
    
    ON_DEBUG
    (
    Stack->Data[Stack->Capacity - 1] = RightDataCanary;
    Stack->DataHash = Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity);
    Stack->StackHash = CalcRealStackHash(Stack);
    
    if (Stack->Data == NULL)
    {
        return REALLOC_POP_NULL;
    }
    return Verif(Stack);
    );

    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

ErrorCode PrintStack(Stack_t* Stack)
{
    // ON_DEBUG (ERROR_RETURN(Verif(Stack)));

    printf("\nStack:\n");
    for (size_t Stack_i = 0 ON_DEBUG(+ 1); Stack_i < Stack->Size; Stack_i++)
    {
        printf("%d ", Stack->Data[Stack_i]);
    }
    printf("\nStack end\n\n");

    ON_DEBUG(return Verif(Stack));
    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
