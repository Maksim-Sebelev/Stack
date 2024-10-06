
#include <stdio.h>
#include <string.h>
#include "Stack.h"
#include "ErrorFunc.h"
#include "ColorPrint.h"
#include "Stack.h"
#include "StackFunc.h"
#include "HashFunc.h"

//----------------------------------------------------------------------------------------------------------------------

ErrorType Verif(Stack_t* Stack, ErrorType* Error ON_DEBUG(, const char* File, int Line, const char* Func))
{
    ON_DEBUG(ErrPlaceCtor(Error, File, Line, Func);)
 
    if (Stack == NULL)
    {
        Error->FatalError.StackNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }  
    else
    {
        Error->FatalError.StackNull = 0;   
    }

    if (Stack->Data == NULL)
    {
        Error->FatalError.DataNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }
    else
    {
        Error->FatalError.DataNull = 0;
    }

    ON_SCANARY
    (
    if (Stack->LeftStackCanary != LeftStackCanary)
    {
        Error->FatalError.LeftStackCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftStackCanaryChanged = 0;
    }

    if (Stack->RightStackCanary != RightStackCanary)
    {
        Error->FatalError.RightStackCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightStackCanaryChanged = 0;
    }
    )

    ON_DCANARY
    (
    if (GetLeftDataCanary(Stack) != LeftDataCanary)
    {
        Error->FatalError.LeftDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftDataCanaryChanged = 0;
    }

    if (GetRightDataCanary(Stack) != RightDataCanary)
    {
        Error->FatalError.RightDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightDataCanaryChanged = 0;
    }
    )

    ON_DEBUG
    (
    if (Stack->Size > Stack->Capacity)
    {
        Error->FatalError.SizeBiggerCapacity = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.SizeBiggerCapacity = 0;   
    }

    if (Stack->Capacity < MinCapacity)
    {
        Error->FatalError.CapacitySmallerMin = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacitySmallerMin = 0;
    }

    if (Stack->Capacity > MaxCapacity)
    {
        Error->FatalError.CapacityBiggerMax = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacityBiggerMax = 0;
    }
    
    if (Stack->Var.File == NULL)
    {
        Error->FatalError.CtorStackFileNull = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CtorStackFileNull = 0;   
    }

    if (Stack->Var.Func == NULL)
    {
        Error->FatalError.CtorStackFuncNull = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CtorStackFuncNull = 0;
    }

    if (Stack->Var.Name == NULL)
    {
        Error->FatalError.CtorStackNameNull = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CtorStackNameNull = 0;
    }

    if (Stack->Var.Line < 0)
    {
        Error->FatalError.CtorStackLineNegative = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CtorStackLineNegative = 0;
    }
    );

    ON_POISON
    (
    int WasNotPosion = 0;
    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity; Data_i++)
    {
        if (Stack->Data[Data_i] != Poison)
        {
            Error->FatalError.DataElemBiggerSizeNotPoison = 1;
            Error->IsFatalError = 1;
            WasNotPosion = 1;
            break;
        }
    }

    if (WasNotPosion == 0)
    {
        Error->FatalError.DataElemBiggerSizeNotPoison = 0;
    }
    )
    
    ON_DHASH
    (
    if (Hash(Stack->Data, sizeof(StackElem_t) * Stack->Capacity) != Stack->DataHash)
    {
        Error->FatalError.DataHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.DataHashChanged = 0;
    }
    );

    ON_SHASH
    (
    if (CalcRealStackHash(Stack) != Stack->StackHash)
    {
        Error->FatalError.StackHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.StackHashChanged = 0;
    }
    );
    return *Error;
}

//----------------------------------------------------------------------------------------------------------------------

void PrintError(ErrorType Error)
{
    if (Error.IsWarning == 0 && Error.IsFatalError == 0)
    {
        return;
    }
    
    if (Error.IsWarning == 1)
    {
        if (Error.Warning.PopInEmptyStack == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: make pop, but Stack is empty.\n");
            COLOR_PRINT(YELLOW, "Pop will not change PopElem.\n");
        }

        if (Error.Warning.ToBigCapacity == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: to big Data size.\n");
            COLOR_PRINT(YELLOW, "Capacity have a max allowed value.\n");
        }

        if (Error.Warning.PushInFullStack == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: made Push in full Stack.\n");
            COLOR_PRINT(YELLOW, "Stack won't change.\n");
        }
    }

    if (Error.IsFatalError == 1)
    {
        if (Error.FatalError.StackNull == 1)
        {
            COLOR_PRINT(RED, "Error: Right Data Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
        }

        if (Error.FatalError.DataNull == 1)
        {
            COLOR_PRINT(RED, "Error: Data is NULL.\n");
        }

        if (Error.FatalError.CallocCtorNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to allocate memory in Ctor.\n");
        }

        if (Error.FatalError.ReallocPushNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to reallocate memory in Push.\n");
        }

        if (Error.FatalError.ReallocPopNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to free memory in Pop.\n");
        }

        ON_SCANARY
        (
        if (Error.FatalError.LeftStackCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left Stack Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
        }
        
        if (Error.FatalError.RightStackCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right Stack Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
        }
        )
        ON_DCANARY
        (
        if (Error.FatalError.LeftDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left Data Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
        }

        if (Error.FatalError.RightDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right Data Canary was changed.\n");
            OFF_DEBUG(COLOR_PRINT(RED, "!Stack Data can be incorrect!\n"));
        }
        )

        ON_POISON
        (
        if (Error.FatalError.DataElemBiggerSizeNotPoison == 1)
        {
            COLOR_PRINT(RED, "Error: After Size in Data is not Poison elem.\n");
        }
        )

        ON_DEBUG
        (
        if (Error.FatalError.SizeBiggerCapacity == 1)
        {
            COLOR_PRINT(RED, "Error: Size > Capacity.\n");
        }
        
        if (Error.FatalError.CapacityBiggerMax == 1)
        {
            COLOR_PRINT(RED, "Error: Capacity > MaxCapacity.\n");
        }

        if (Error.FatalError.CapacitySmallerMin == 1)
        {
            COLOR_PRINT(RED, "Error: Capacity < MinCapacity.\n");
        }

        if (Error.FatalError.CtorStackFileNull == 1)
        {
            COLOR_PRINT(RED, "Error: Stack ctor init file is NULL.\n");
        }

        if (Error.FatalError.CtorStackFuncNull == 1)
        {
            COLOR_PRINT(RED, "Error: Stack ctor init func is NULL.\n");
        }
        
        if (Error.FatalError.CtorStackLineNegative == 1)
        {
            COLOR_PRINT(RED, "Error: Stack ctor init line is negative or 0.\n");
        }
        )

        ON_DHASH
        (
        if (Error.FatalError.DataHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Data Hash is incorrect.\n");
        }
        )

        ON_SHASH
        (
        if (Error.FatalError.StackHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Stack Hash is incorrect.\n");
        }
        )
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------

void Dump(Stack_t* Stack, const char* File, int Line, const char* Func)
{   
    COLOR_PRINT(GREEN, "\nDump BEGIN\n\n");
    COLOR_PRINT(VIOLET, "Where Dump made:\n");
    PrintPlace(File, Line, Func);
    ON_DEBUG
    (
    #define DANG_DUMP

    #ifndef DANG_DUMP
        ON_SHASH
        (
        if (Stack->StackHash != CalcRealStackHash(Stack))
        {
            COLOR_PRINT(RED, "Incorrect hash.\n");
            COLOR_PRINT(WHITE, "Correct hash    = %d\n", CalcRealStackHash(Stack));
            COLOR_PRINT(WHITE, "Stack.StackHash = %d\n", Stack->StackHash);
            return;
        }
        )
    #else
        COLOR_PRINT(YELLOW, "WARNING: Dump is in dangerous mode.\n");
        COLOR_PRINT(YELLOW, "Undefined bahavior is possible.\n\n");
    #endif
    )

    COLOR_PRINT(VIOLET, "Stack data during Ctor:\n");

    if (Stack == NULL)
    {
        COLOR_PRINT(RED, "Stack = NULL\n");
        return;
    }

    ON_DEBUG
    (
    if (&Stack->Var == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var = NULL\n");
        return;
    }

    if (Stack->Var.Name == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var.Name = NULL\n");
    }
    else
    {
        COLOR_PRINT(WHITE, "Name [%s]\n", Stack->Var.Name);
    }

    if (Stack->Var.File == NULL)
    {
        COLOR_PRINT(RED, "Stack.Var.File = NULL\n");
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

    COLOR_PRINT(VIOLET, "&Stack = 0x%p\n", Stack);
    COLOR_PRINT(VIOLET, "&Data  = 0x%p\n\n", Stack->Data);

    ON_SCANARY
    (
    COLOR_PRINT(YELLOW, "Left  Stack Canary = 0x%llx = %llu\n", Stack->LeftStackCanary, Stack->LeftStackCanary);
    COLOR_PRINT(YELLOW, "Right Stack Canary = 0x%llx = %llu\n\n", Stack->RightStackCanary, Stack->RightStackCanary);
    )
    ON_DCANARY
    (
    COLOR_PRINT(YELLOW, "Left  Data  Canary = 0x%llx = %llu\n", GetLeftDataCanary(Stack), GetLeftDataCanary(Stack));
    COLOR_PRINT(YELLOW, "Right Data  Canary = 0x%llx = %llu\n\n", GetRightDataCanary(Stack), GetRightDataCanary(Stack));
    )

    ON_SHASH(COLOR_PRINT(BLUE, "Stack Hash = %llu\n", Stack->StackHash);)
    ON_DHASH(COLOR_PRINT(BLUE, "Data  Hash = %llu\n\n", Stack->DataHash);)

    COLOR_PRINT(CYAN, "Size = %u\n", Stack->Size);
    COLOR_PRINT(CYAN, "Capacity = %u\n\n", Stack->Capacity);


    ON_POISON(COLOR_PRINT(GREEN, "Poison = 0x%x = %d\n\n", Poison, Poison);)

    COLOR_PRINT(BLUE, "Data = \n{\n");
    for (size_t Data_i = 0; Data_i < Stack->Size; Data_i++)
    {
        COLOR_PRINT(BLUE, "*[%2u] %d\n", Data_i, Stack->Data[Data_i]);
    }

    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity; Data_i++)
    {
        COLOR_PRINT(CYAN, " [%2u] 0x%x\n", Data_i, Stack->Data[Data_i]);   
    }
    COLOR_PRINT(BLUE, "};\n\n");


    COLOR_PRINT(VIOLET, "Data ptrs = \n{\n");
    for (size_t Data_i = 0; Data_i < Stack->Size; Data_i++)
    {
        COLOR_PRINT(VIOLET, "*[%2u] 0x%p\n", Data_i, &Stack->Data[Data_i]);
    }

    for (size_t Data_i = Stack->Size; Data_i < Stack->Capacity; Data_i++)
    {
        COLOR_PRINT(CYAN, " [%2u] 0x%p\n", Data_i, &Stack->Data[Data_i]);   
    }
    COLOR_PRINT(VIOLET, "};\n");

    COLOR_PRINT(GREEN, "\n\nDump END\n\n");
    return;
}

//----------------------------------------------------------------------------------------------------------------------

void PrintPlace(const char* File, const int Line, const char* Function)
{
    COLOR_PRINT(WHITE, "File [%s]\nLine [%d]\nFunc [%s]\n", File, Line, Function);
    return;
}

//----------------------------------------------------------------------------------------------------------------------

void AssertPrint(ErrorType Err, const char* File, int Line, const char* Func)
{
    if (Err.IsFatalError == 1 || Err.IsWarning == 1) 
    {
        // COLOR_PRINT(WHITE, "\n"
        // "//----------------------------------------------------------------------------------------------------\n\n");
        COLOR_PRINT(RED, "Assert made in:\n");
        PrintPlace(File, Line, Func);
        PrintError(Err);
        ON_DEBUG(PrintPlace(Err.File, Err.Line, Err.Func);)
        printf("\n");
        // COLOR_PRINT(WHITE, "\n"
        // "//----------------------------------------------------------------------------------------------------\n\n");
    }
}

ON_DEBUG
(
void ErrPlaceCtor(ErrorType* Err, const char* File, int Line, const char* Func)
{
    Err->File = File;
    Err->Line = Line;
    Err->Func = Func;
    return;
}
)
