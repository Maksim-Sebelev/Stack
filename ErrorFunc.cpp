
#include <stdio.h>
#include <string.h>
#include "Stack.h"
#include "ErrorFunc.h"
#include "ColorPrint.h"
#include "Stack.h"
#include "StackFunc.h"
#include "HashFunc.h"


//----------------------------------------------------------------------------------------------------------------------

ErrorCode Verif(Stack_t* Stack)
{
    if (Stack->LeftStackCanary != LeftStackCanary)
    {
        return LEFT_STACK_CANARY_CHANGED;
    }

    if (Stack->RightStackCanary != RightStackCanary)
    {
        return RIGHT_STACK_CANARY_CHANGED;
    }
    
    if (Stack->Data[0] != LeftDataCanary)
    {
        return LEFT_DATA_CANARY_CHANGED;
    }

    if (Stack->Data[Stack->Capacity - 1] != RightDataCanary)
    {
        return RIGHT_DATA_CANARY_CHANGED;
    }


    ON_DEBUG
    (
    if (&Stack == NULL)
    {
        return STACK_NULL;
    }  

    if (Stack->Data == NULL)
    {
        return DATA_NULL;
    }

    if (Stack->Size > Stack->Capacity + ON_DEBUG(+ 1))
    {
        return SIZE_BIGGER_CAPACITY;
    }

    if (Stack->Capacity < MinCapacity)
    {
        return CAPACITY_SMALLER_MIN;
    }

    if (Stack->Capacity > MaxCapacity)
    {
        return CAPACITY_BIGGER_MAX;
    }

    if (Stack->Var.File == NULL)
    {
        return STACK_CTOR_FILE_NULL;
    }

    if (Stack->Var.Func == NULL)
    {
        return STACK_CTOR_FUNC_NULL;
    }

    if (Stack->Var.Name == NULL)
    {
        return STACK_CTOR_NAME_NULL;
    }

    if (Stack->Var.Line < 0)
    {
        return STACK_CTOR_LINE_NEGATIVE;
    }

    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity - 1; Data_i++)
    {
        if (Stack->Data[Data_i] != Poison)
        {
            return DATA_ELEM_BIGGER_SIZE_IS_NOT_POISON;
        }
    }

    if (Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity) != Stack->DataHash)
    {
        return DATA_HASH_CHANGED;
    }

    if (CalcRealStackHash(Stack) != Stack->StackHash)
    {
        return STACK_HASH_CHANGED;
    }
    );
    return NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

void PrintError(const ErrorCode Error, const char* File, const int Line, const char* Function)
{
    switch (Error)
    {
        case NO_ERR:
            return;
            break;

        case POP_IN_EMPTY_STACK:
            COLOR_PRINT(YELLOW, "\nWarning: make pop, but Stack is empty.\n");
            COLOR_PRINT(YELLOW, "Pop will not change PopElem.\n");
            PrintPlace(File, Line, Function);
            break;

        case TO_BIG_CAPACITY:
            COLOR_PRINT(YELLOW, "\nWarning: to big Data size.\n");
            COLOR_PRINT(YELLOW, "Capacity have a max allowed value.\n");
            PrintPlace(File, Line, Function);
            break;
        
        case PUSH_IN_FULL_STACK:
            COLOR_PRINT(RED, "\nError: made Push in full Stack.\n");
            COLOR_PRINT(RED, "Capacity = MaxCapcity and Size = Capacity.\n");
            PrintPlace(File, Line, Function);
            break;

        case LEFT_STACK_CANARY_CHANGED:
            COLOR_PRINT(RED, "\nError: Left Stack Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
            PrintPlace(File, Line, Function);
            break;

        case RIGHT_STACK_CANARY_CHANGED:
            COLOR_PRINT(RED, "\nError: Right Stack Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
            PrintPlace(File, Line, Function);
            break;
        
        case LEFT_DATA_CANARY_CHANGED:
            COLOR_PRINT(RED, "\nError: Left Data Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
            PrintPlace(File, Line, Function);
            break;
        
        case RIGHT_DATA_CANARY_CHANGED:
            COLOR_PRINT(RED, "\nError: Right Data Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
            PrintPlace(File, Line, Function);
            break;

        ON_DEBUG
        (
        case STACK_NULL:
            COLOR_PRINT(RED, "\nError: Stack ptr is NULL.\n");
            PrintPlace(File, Line, Function);
            break;

        case DATA_NULL:
            COLOR_PRINT(RED, "\nError: Data is NULL.\n");
            PrintPlace(File, Line, Function);
            break;

        case SIZE_BIGGER_CAPACITY:
            COLOR_PRINT(RED, "\nError: Size > Capacity.\n");
            PrintPlace(File, Line, Function);
            break;

        case CALLOC_CTOR_NULL:
            COLOR_PRINT(RED, "\nError: failed to allocate memory in Ctor.\n");
            PrintPlace(File, Line, Function);
            break;
        
        case REALLOC_PUSH_NULL:
            COLOR_PRINT(RED, "\nError: failed to reallocate memory in Push.\n");
            PrintPlace(File, Line, Function);
            break;

        case REALLOC_POP_NULL:
            COLOR_PRINT(RED, "\nError: failed to free memory in Pop.\n");
            PrintPlace(File, Line, Function);
            break;

        case CAPACITY_BIGGER_MAX:
            COLOR_PRINT(RED, "\nError: Capacity > MaxCapacity.\n");
            PrintPlace(File, Line, Function);
            break;

        case CAPACITY_SMALLER_MIN:
            COLOR_PRINT(RED, "\nError: Capacity < MinCapacity.\n");
            PrintPlace(File, Line, Function);
            break;

        case DATA_ELEM_BIGGER_SIZE_IS_NOT_POISON:
            COLOR_PRINT(RED, "\nError: After Size in Data is not Poison elem.\n");
            PrintPlace(File, Line, Function);
            break;

        case STACK_HASH_CHANGED:
            COLOR_PRINT(RED, "\nError: Stack Hash is incorrect.\n");
            PrintPlace(File, Line, Function);
            break;
        
        case DATA_HASH_CHANGED:
            COLOR_PRINT(RED, "\nError: Data Hash is incorrect.\n");
            PrintPlace(File, Line, Function);
            break;
        
        case STACK_CTOR_FILE_NULL:
            COLOR_PRINT(RED, "\nError: Stack ctor init file is NULL.\n");
            PrintPlace(File, Line, Function);
            break;

        case STACK_CTOR_FUNC_NULL:
            COLOR_PRINT(RED, "\nError: Stack ctor init func is NULL.\n");
            PrintPlace(File, Line, Function);
            break;
        
        case STACK_CTOR_LINE_NEGATIVE:
            COLOR_PRINT(RED, "\nError: Stack ctor init line is negative or 0.\n");
            PrintPlace(File, Line, Function);
            break;
        );
        
        default:
            ON_DEBUG(COLOR_PRINT(RED, "\nUndefined situation (maybe autor forgot about same ErrorCode).\n"));
            break;
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------------

void Dump(Stack_t* Stack)
{   
    ON_DEBUG
    (
    // #define DANG_DUMP

    #ifndef DANG_DUMP
        if (Stack->StackHash != CalcRealStackHash(Stack))
        {
            COLOR_PRINT(RED, "Incorrect hash.\n");
            return;
        }
    #else
        COLOR_PRINT(YELLOW, "WARNING: Dump is dangerius mode.\n");
        COLOR_PRINT(YELLOW, "Undefined bahavior is possible.\n\n");
    #endif
    );

    COLOR_PRINT(VIOLET, "Stack data during Ctor:\n");

    if (&Stack == NULL)
    {
        COLOR_PRINT(RED, "Stack = NULL\n");
        return;
    }

    ON_DEBUG
    (
    if (&Stack->Var == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var ptr is NULL\n");
        return;
    }

    if (Stack->Var.Name == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var.Name is NULL\n");
    }
    else
    {
        COLOR_PRINT(WHITE, "Name [%s]\n", Stack->Var.Name);
    }

    if (Stack->Var.File == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var.File is NULL\n");
    }
    else
    {
        COLOR_PRINT(WHITE, "File [%s]\n", Stack->Var.File);
    }

    if (Stack->Var.Func == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var.Func is NULL\n");
    }
    else
    {
        COLOR_PRINT(WHITE, "Func [%s]\n", Stack->Var.Func);
    }


    COLOR_PRINT(WHITE, "Line [%d]\n\n", Stack->Var.Line);
    );

    if (Stack->Data == NULL)
    {
        COLOR_PRINT(RED, "Stack.Data = NULL");
        return;
    }

    COLOR_PRINT(VIOLET, "&Stack = 0x%p\n", &Stack);
    COLOR_PRINT(VIOLET, "&Data  = 0x%p\n\n", Stack->Data);

    COLOR_PRINT(YELLOW, "Left  Stack Canary = 0x%8x = %8d\n", Stack->LeftStackCanary, Stack->LeftStackCanary);
    COLOR_PRINT(YELLOW, "Right Stack Canary = 0x%8x = %8d\n\n", Stack->RightStackCanary, Stack->RightStackCanary);

    COLOR_PRINT(YELLOW, "Left  Data  Canary = 0x%8x = %8d\n", Stack->Data[0], Stack->Data[0]);
    COLOR_PRINT(YELLOW, "Right Data  Canary = 0x%8x = %8d\n\n", Stack->Data[Stack->Capacity - 1], Stack->Data[Stack->Capacity - 1]);

    ON_DEBUG
    (
    COLOR_PRINT(BLUE, "Stack Hash = %d\n", Stack->StackHash);
    COLOR_PRINT(BLUE, "Data  Hash = %d\n\n", Stack->DataHash);
    );

    COLOR_PRINT(CYAN, "Size = %u\n", Stack->Size);
    COLOR_PRINT(CYAN, "Capacity = %u\n\n", Stack->Capacity);


    COLOR_PRINT(GREEN, "Poison = %x = %d\n\n", Poison, Poison);

    COLOR_PRINT(BLUE, "Data = \n{\n");
    for (size_t Data_i = 1; Data_i < Stack->Size; Data_i++)
    {
        COLOR_PRINT(BLUE, "*[%2u] %d\n", Data_i - 1, Stack->Data[Data_i]);
    }

    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity - 1; Data_i++)
    {
        COLOR_PRINT(CYAN, " [%2u] 0x%x\n", Data_i - 1, Stack->Data[Data_i]);   
    }
    COLOR_PRINT(BLUE, "};\n\n");


    COLOR_PRINT(VIOLET, "Data ptrs = \n{\n");
    for (size_t Data_i = 1; Data_i < Stack->Size; Data_i++)
    {
        COLOR_PRINT(VIOLET, "*[%2u] 0x%p\n", Data_i - 1, &Stack->Data[Data_i]);
    }

    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity - 1; Data_i++)
    {
        COLOR_PRINT(CYAN, " [%2u] 0x%p\n", Data_i - 1, &Stack->Data[Data_i]);   
    }
    COLOR_PRINT(VIOLET, "};\n");
    return;
}

//----------------------------------------------------------------------------------------------------------------------

void PrintPlace(const char* File, const int Line, const char* Function)
{
    COLOR_PRINT(WHITE, "File [%s]\nLine [%d]\nFunc [%s]\n\n", File, Line, Function);
    return;
}

//----------------------------------------------------------------------------------------------------------------------
