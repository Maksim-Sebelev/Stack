#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "Stack.hpp"
#include "Hash.hpp"
#include "ColorPrint.hpp"

static const size_t MinCapacity = 1<<3;
static const size_t MaxCapacity = 1<<21;

static const unsigned int CapPushReallocCoef = 2;
static const unsigned int CapPopReallocCoef  = 4;

ON_STACK_DATA_CANARY
(
typedef uint64_t DataCanary_t;
)

ON_STACK_DATA_CANARY
(
static const StackCanary_t leftStackCanary  = 0xDEEADDEADDEAD;
static const StackCanary_t rightStackCanary = 0xDEADDEDDEADED;
)
ON_STACK_DATA_CANARY
(
static const DataCanary_t LeftDataCanary    = 0xEDADEDAEDADEDA;
static const DataCanary_t RightDataCanary   = 0xDEDDEADDEDDEAD;
)
ON_STACK_HASH
(
static const uint64_t DefaultStackHash = 538176576;
)
ON_STACK_DATA_POISON
(
static const StackElem_t Poison = 0xDEEEEEAD;
)

static size_t GetNewCapacity      (size_t capacity); 
static size_t GetNewCtorCapacity  (size_t StackDataSize);
static size_t GetNewPushCapacity  (const Stack_t* stack);
static size_t GetNewPopCapacity   (const Stack_t* stack);

static StackErrorType CtorCalloc   (Stack_t* stack, size_t StackDataSize);
static StackErrorType DtorFreeData (Stack_t* stack);
static StackErrorType PushRealloc  (Stack_t* stack);
static StackErrorType PopRealloc   (Stack_t* stack);

ON_STACK_DATA_CANARY
(
static DataCanary_t GetLeftDataCanary    (const Stack_t* stack);
static DataCanary_t GetRightDataCanary   (const Stack_t* stack);
static void         SetLeftDataCanary    (Stack_t* stack);
static void         SetRightDataCanary   (Stack_t* stack);
static StackErrorType    MoveDataToLeftCanary (Stack_t* stack);
static StackErrorType    MoveDataToFirstElem  (Stack_t* stack);
)

ON_STACK_DATA_HASH
(
static uint64_t CalcDataHash(const Stack_t* stack);
)
ON_STACK_HASH
(
static uint64_t CalcStackHash (Stack_t* stack);
)

static StackErrorType Verif       (Stack_t* stack, StackErrorType* Error ON_STACK_DEBUG(, const char* file, int line, const char* func));
static void      PrintError  (StackErrorType Error);
static void      PrintPlace  (const char* file, int line, const char* Function);
ON_STACK_DEBUG
(
static void      ErrPlaceCtor (StackErrorType* err, const char* file, int line, const char* func);
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define STACK_VERIF(StackPtr, err) Verif(StackPtr, &err ON_STACK_DEBUG(, __FILE__, __LINE__, __func__))

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RETURN_IF_ERR_OR_WARN(StackPtr, err) do                             \
{                                                                            \
    StackErrorType ErrCopy = err;                                                  \
    Verif(stack, &ErrCopy ON_STACK_DEBUG(, __FILE__, __LINE__, __func__));     \
    if (ErrCopy.IsFatalError == 1 || ErrCopy.IsWarning == 1)                    \
    {                                                                            \
        return ErrCopy;                                                           \
    }                                                                              \
} while (0)                                                                         \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType StackCtor(Stack_t* stack, size_t StackDataSize)
{
    StackErrorType err = {};

    stack->size = 0;

    stack->capacity = GetNewCtorCapacity(StackDataSize);
    STACK_ASSERT(CtorCalloc(stack, StackDataSize));

    ON_STACK_DATA_CANARY
    (
    stack->leftStackCanary  = leftStackCanary;
    stack->rightStackCanary = rightStackCanary;
    )

    ON_STACK_DATA_CANARY
    (
    SetLeftDataCanary (stack);
    SetRightDataCanary(stack);
    )

    ON_STACK_DATA_POISON
    (
    for (size_t data_i = 0; data_i < stack->capacity; data_i++)
    {
        stack->data[data_i] = Poison;
    }
    )

    ON_STACK_DATA_HASH(stack->dataHash  = CalcDataHash(stack);)
    ON_STACK_HASH(stack->stackHash = CalcStackHash(stack);)

    return STACK_VERIF(stack, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType StackDtor(Stack_t* stack)
{
    assert(stack);
    StackErrorType err   = {};
    STACK_ASSERT(DtorFreeData(stack));
    stack->data     = nullptr;
    stack->capacity = 0;
    stack->size     = 0;
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType StackPush(Stack_t*  stack, StackElem_t PushElem)
{
    StackErrorType err = {};
    RETURN_IF_ERR_OR_WARN(stack, err);

    if (stack->size + 1 > MaxCapacity)
    {
        err.Warning.PushInFullStack = 1;
        err.IsWarning = 1;
        return STACK_VERIF(stack, err);
    }

    stack->size++;

    if (stack->size <= stack->capacity)
    {
        stack->data[stack->size - 1] = PushElem;
    
        ON_STACK_DATA_HASH(stack->dataHash  = CalcDataHash(stack);)
        ON_STACK_HASH(stack->stackHash = CalcStackHash(stack);)

        return STACK_VERIF(stack, err);
    }

    stack->capacity = GetNewPushCapacity(stack);
    STACK_ASSERT(PushRealloc(stack));

    if (err.IsFatalError == 1)
    {
        return STACK_VERIF(stack, err);
    }

    stack->data[stack->size - 1] = PushElem;

    ON_STACK_DATA_CANARY(SetRightDataCanary(stack);)

    ON_STACK_DATA_POISON
    (
    for (size_t data_i = stack->size; data_i < stack->capacity; data_i++)
    {
        stack->data[data_i] = Poison;
    }
    )

    ON_STACK_DATA_HASH(stack->dataHash  = CalcDataHash(stack);)
    ON_STACK_HASH(stack->stackHash = CalcStackHash(stack);)
    
    if (stack->capacity == MaxCapacity)
    {
        err.Warning.TooBigCapacity = 1;
        err.IsWarning = 1;
    }
    return STACK_VERIF(stack, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType StackPop(Stack_t* stack, StackElem_t* PopElem)
{
    StackErrorType err = {};
    RETURN_IF_ERR_OR_WARN(stack, err);

    if (stack->size == 0)
    {
        err.Warning.PopInEmptyStack = 1;
        err.IsWarning = 1;
        return STACK_VERIF(stack, err);
    }

    stack->size--;

    *PopElem = stack->data[stack->size];

    ON_STACK_DATA_POISON(stack->data[stack->size] = Poison;)
    ON_STACK_DATA_HASH(stack->dataHash  = CalcDataHash(stack);)
    ON_STACK_HASH(stack->stackHash = CalcStackHash(stack);)

    if (stack->size * CapPopReallocCoef > stack->capacity)
    {
        return STACK_VERIF(stack, err);
    }

    stack->capacity = GetNewPopCapacity(stack);
    STACK_ASSERT(PopRealloc(stack));

    if (err.IsFatalError == 1)
    {
        return STACK_VERIF(stack, err);
    }

    ON_STACK_DATA_CANARY(SetRightDataCanary(stack);)
    ON_STACK_DATA_HASH(stack->dataHash  = CalcDataHash(stack);)
    ON_STACK_HASH(stack->stackHash = CalcStackHash(stack);)

    return STACK_VERIF(stack, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType PrintStack(Stack_t* stack)
{
    StackErrorType err = {};
    RETURN_IF_ERR_OR_WARN(stack, err);

    printf("\nStack:\n");
    for (size_t Stack_i = 0; Stack_i < stack->size; Stack_i++)
    {
        printf("%d ", stack->data[Stack_i]);
    }
    printf("\nStack end\n\n");

    return STACK_VERIF(stack, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StackErrorType PrintLastStackElem(Stack_t* stack)
{
    StackErrorType err = {};
    RETURN_IF_ERR_OR_WARN(stack, err);
    COLOR_PRINT(WHITE, "Last stack Elem = %d\n", stack->data[stack->size - 1]);
    return STACK_VERIF(stack, err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_HASH
(
static uint64_t CalcStackHash(Stack_t* stack)
{
    assert(stack);

    const uint64_t StackHashCopy = stack->stackHash;
    stack->stackHash             = DefaultStackHash;
    uint64_t NewStackHash        = Hash(stack, 1, sizeof(Stack_t));
    stack->stackHash             = StackHashCopy;
    return NewStackHash;
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_DATA_HASH
(
static uint64_t CalcDataHash(const Stack_t* stack)
{
    assert(stack);

    return Hash(stack->data, stack->capacity, sizeof(StackElem_t));
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_DATA_CANARY
(
static DataCanary_t GetLeftDataCanary(const  Stack_t* stack)
{
    assert(stack);

    return *(DataCanary_t*)((char*)stack->data - 1 * sizeof(DataCanary_t));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DataCanary_t GetRightDataCanary(const Stack_t* stack)
{
    assert(stack);

    return *(DataCanary_t*)((char*)stack->data + stack->capacity * sizeof(StackElem_t));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void SetLeftDataCanary(Stack_t* stack)
{
    assert(stack);

    *(DataCanary_t*)((char*)stack->data - 1 * sizeof(DataCanary_t)) = LeftDataCanary;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void SetRightDataCanary(Stack_t* stack)
{
    assert(stack);

    *(DataCanary_t*)((char*)stack->data + stack->capacity * sizeof(StackElem_t)) = RightDataCanary;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType MoveDataToLeftCanary(Stack_t* stack)
{
    assert(stack);

    StackErrorType err = {};

    stack->data = (StackElem_t*)((char*)stack->data - sizeof(LeftDataCanary) * sizeof(char));

    if (stack->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return STACK_VERIF(stack, err);
    }

    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType MoveDataToFirstElem(Stack_t* stack)
{
    assert(stack);

    StackErrorType err = {};

    stack->data = (StackElem_t*)((char*)stack->data + sizeof(RightDataCanary) * sizeof(char));

    if (stack->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return STACK_VERIF(stack, err);
    }

    return err;
}
)
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewCtorCapacity(const size_t StackDataSize)
{
    size_t Temp = (StackDataSize > MinCapacity) ? StackDataSize : MinCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPushCapacity(const Stack_t* stack)
{
    assert(stack);

    size_t NewCapacity = (stack->capacity * CapPushReallocCoef);
    size_t Temp = NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPopCapacity(const Stack_t* stack)
{
    assert(stack);

    size_t NewCapacity = (stack->capacity / CapPopReallocCoef);
    size_t Temp = (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
    return GetNewCapacity(Temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewCapacity(size_t capacity)
{
    ON_STACK_DATA_CANARY
    (
    size_t DataCanarySize = sizeof(DataCanary_t);
    size_t Temp = (capacity % DataCanarySize == 0) ? 0 : DataCanarySize - capacity % DataCanarySize;
    capacity += Temp;
    )
    return capacity;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType PushRealloc(Stack_t* stack)
{
    assert(stack);
    StackErrorType err = {};

    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToLeftCanary(stack));
    )
    stack->data = (StackElem_t*) realloc(stack->data, stack->capacity * sizeof(StackElem_t) ON_STACK_DATA_CANARY(+ 2 * sizeof(DataCanary_t)));
    if (stack->data == nullptr)
    {
        err.FatalError.ReallocPushNull = 1;
        err.IsFatalError = 1;
        return STACK_VERIF(stack, err);
    }
    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToFirstElem(stack));
    )
    return err; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType PopRealloc(Stack_t* stack)
{
    assert(stack);

    StackErrorType err = {};

    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToLeftCanary(stack));
    )
    stack->data = (StackElem_t*) realloc(stack->data, stack->capacity * sizeof(StackElem_t) ON_STACK_DATA_CANARY(+ 2 * sizeof(DataCanary_t)));
    if (stack->data == nullptr)
    {
        err.FatalError.ReallocPopNull = 1;
        err.IsFatalError = 1;
        return STACK_VERIF(stack, err);
    }
    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToFirstElem(stack));
    )
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType CtorCalloc(Stack_t* stack, size_t StackDataSize)
{
    StackErrorType err = {};
    assert(stack);

    stack->data = (StackElem_t*) calloc (stack->capacity * sizeof(StackElem_t) ON_STACK_DATA_CANARY(+ 2 * sizeof(DataCanary_t)), sizeof(char));

    if (stack->data == nullptr)
    {
        err.FatalError.DataNull = 1;
        err.IsFatalError = 1;
        return STACK_VERIF(stack, err);
    }
    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToFirstElem(stack));
    )
    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType DtorFreeData(Stack_t* stack)
{
    StackErrorType err = {};
    RETURN_IF_ERR_OR_WARN(stack, err);

    ON_STACK_DATA_CANARY
    (
    STACK_ASSERT(MoveDataToLeftCanary(stack));
    )
    free(stack->data);
    stack->data = nullptr;

    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static StackErrorType Verif(Stack_t* stack, StackErrorType* Error ON_STACK_DEBUG(, const char* file, int line, const char* func))
{
    assert(Error);

    ON_STACK_DEBUG
    (
    ErrPlaceCtor(Error, file, line, func);
    )

    if (stack == nullptr)
    {
        Error->FatalError.StackNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }
    else
    {
        Error->FatalError.StackNull = 0;   
    }

    if (stack->data == nullptr)
    {
        Error->FatalError.DataNull = 1;
        Error->IsFatalError = 1;
        return *Error;
    }
    else
    {
        Error->FatalError.DataNull = 0;
    }

    ON_STACK_DATA_CANARY
    (
    if (stack->leftStackCanary != leftStackCanary)
    {
        Error->FatalError.LeftStackCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftStackCanaryChanged = 0;
    }

    if (stack->rightStackCanary != rightStackCanary)
    {
        Error->FatalError.RightStackCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightStackCanaryChanged = 0;
    }
    )

    ON_STACK_DATA_CANARY
    (
    if (GetLeftDataCanary(stack) != LeftDataCanary)
    {
        Error->FatalError.LeftDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.LeftDataCanaryChanged = 0;
    }

    if (GetRightDataCanary(stack) != RightDataCanary)
    {
        Error->FatalError.RightDataCanaryChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.RightDataCanaryChanged = 0;
    }
    )

    ON_STACK_DEBUG
    (
    if (stack->size > stack->capacity)
    {
        Error->FatalError.SizeBiggerCapacity = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.SizeBiggerCapacity = 0;   
    }

    if (stack->capacity < MinCapacity)
    {
        Error->FatalError.CapacitySmallerMin = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacitySmallerMin = 0;
    }

    if (stack->capacity > MaxCapacity)
    {
        Error->FatalError.CapacityBiggerMax = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.CapacityBiggerMax = 0;
    }
    )

    ON_STACK_DATA_POISON
    (
    bool WasNotPosion = false;
    for (size_t data_i = stack->size; data_i < stack->capacity; data_i++)
    {
        if (stack->data[data_i] != Poison)
        {
            Error->FatalError.DataElemBiggerSizeNotPoison = 1;
            Error->IsFatalError = 1;
            WasNotPosion = true;
            break;
        }
    }

    if (!WasNotPosion)
    {
        Error->FatalError.DataElemBiggerSizeNotPoison = 0;
    }
    )

    ON_STACK_DATA_HASH
    (
    if (CalcDataHash(stack) != stack->dataHash)
    {
        Error->FatalError.DataHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.DataHashChanged = 0;
    }
    )

    ON_STACK_HASH
    (
    if (CalcStackHash(stack) != stack->stackHash)
    {
        Error->FatalError.StackHashChanged = 1;
        Error->IsFatalError = 1;
    }
    else
    {
        Error->FatalError.StackHashChanged = 0;
    }
    )
    return *Error;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintError(StackErrorType Error)
{
    if (Error.IsWarning == 0 && Error.IsFatalError == 0)
    {
        return;
    }
    
    if (Error.IsWarning == 1)
    {
        if (Error.Warning.PopInEmptyStack == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: make pop, but stack is empty.\n");
            COLOR_PRINT(YELLOW, "StackPop will not change PopElem.\n");
        }

        if (Error.Warning.TooBigCapacity == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: to big data size.\n");
            COLOR_PRINT(YELLOW, "capacity have a max allowed value.\n");
        }

        if (Error.Warning.PushInFullStack == 1)
        {
            COLOR_PRINT(YELLOW, "Warning: made StackPush in full stack.\n");
            COLOR_PRINT(YELLOW, "stack won't change.\n");
        }
    }

    if (Error.IsFatalError == 1)
    {
        if (Error.FatalError.StackNull == 1)
        {
            COLOR_PRINT(RED, "Error: Right data Canary was changed.\n");
            OFF_STACK_DEBUG(COLOR_PRINT(RED, "!stack data can be incorrect!\n"));
        }

        if (Error.FatalError.DataNull == 1)
        {
            COLOR_PRINT(RED, "Error: data is nullptr.\n");
        }

        if (Error.FatalError.CallocCtorNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to allocate memory in StackCtor.\n");
        }

        if (Error.FatalError.ReallocPushNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to reallocate memory in StackPush.\n");
        }

        if (Error.FatalError.ReallocPopNull == 1)
        {
            COLOR_PRINT(RED, "Error: failed to free memory in StackPop.\n");
        }

        ON_STACK_DATA_CANARY
        (
        if (Error.FatalError.LeftStackCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left stack Canary was changed.\n");
            OFF_STACK_DEBUG(COLOR_PRINT(RED, "!stack data can be incorrect!\n"));
        }
        
        if (Error.FatalError.RightStackCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right stack Canary was changed.\n");
            OFF_STACK_DEBUG(COLOR_PRINT(RED, "!stack data can be incorrect!\n"));
        }
        )
        ON_STACK_DATA_CANARY
        (
        if (Error.FatalError.LeftDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Left data Canary was changed.\n");
            OFF_STACK_DEBUG(COLOR_PRINT(RED, "!stack data can be incorrect!\n"));
        }

        if (Error.FatalError.RightDataCanaryChanged == 1)
        {
            COLOR_PRINT(RED, "Error: Right data Canary was changed.\n");
            OFF_STACK_DEBUG(COLOR_PRINT(RED, "!stack data can be incorrect!\n"));
        }
        )

        ON_STACK_DATA_POISON
        (
        if (Error.FatalError.DataElemBiggerSizeNotPoison == 1)
        {
            COLOR_PRINT(RED, "Error: After size in data is not Poison elem.\n");
        }
        )

        ON_STACK_DEBUG
        (
        if (Error.FatalError.SizeBiggerCapacity == 1)
        {
            COLOR_PRINT(RED, "Error: size > capacity.\n");
        }
        
        if (Error.FatalError.CapacityBiggerMax == 1)
        {
            COLOR_PRINT(RED, "Error: capacity > MaxCapacity.\n");
        }

        if (Error.FatalError.CapacitySmallerMin == 1)
        {
            COLOR_PRINT(RED, "Error: capacity < MinCapacity.\n");
        }

        if (Error.FatalError.CtorStackFileNull == 1)
        {
            COLOR_PRINT(RED, "Error: stack ctor init file is nullptr.\n");
        }

        if (Error.FatalError.CtorStackFuncNull == 1)
        {
            COLOR_PRINT(RED, "Error: stack ctor init func is nullptr.\n");
        }
        
        if (Error.FatalError.CtorStackLineNegative == 1)
        {
            COLOR_PRINT(RED, "Error: stack ctor init line is negative or 0.\n");
        }
        )

        ON_STACK_DATA_HASH
        (
        if (Error.FatalError.DataHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: data Hash is incorrect.\n");
        }
        )

        ON_STACK_HASH
        (
        if (Error.FatalError.StackHashChanged == 1)
        {
            COLOR_PRINT(RED, "Error: stack Hash is incorrect.\n");
        }
        )
    }
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_DEBUG
(
void Dump(const Stack_t* stack, const char* file, int line, const char* func)
{   
    COLOR_PRINT(GREEN, "\nDump BEGIN\n\n");

    #define DANG_DUMP

    #ifndef DANG_DUMP
        ON_STACK_HASH
        (
        if (stack->stackHash != CalcStackHash(stack))
        {
            COLOR_PRINT(RED, "Incorrect hash.\n");
            COLOR_PRINT(WHITE, "Correct hash    = %d\n", CalcStackHash(stack));
            COLOR_PRINT(WHITE, "stack.stackHash = %d\n", stack->stackHash);
            return;
        }
        )
    #else
        COLOR_PRINT(YELLOW, "WARNING: Dump is in dangerous mode.\n");
        COLOR_PRINT(YELLOW, "Undefined bahavior is possible.\n\n");
    #endif

    #undef DANG_DUMP

    COLOR_PRINT(VIOLET, "Where Dump made:\n");
    PrintPlace(file, line, func);

    COLOR_PRINT(VIOLET, "stack data during StackCtor:\n");

    if (stack == nullptr)
    {
        COLOR_PRINT(RED, "stack = nullptr\n");
        return;
    }

    if (stack->data == nullptr)
    {
        COLOR_PRINT(RED, "stack.data = nullptr");
        return;
    }

    COLOR_PRINT(VIOLET, "&stack = 0x%p\n", stack);
    COLOR_PRINT(VIOLET, "&data  = 0x%p\n\n", stack->data);

    ON_STACK_DATA_CANARY
    (
    COLOR_PRINT(YELLOW, "Left  stack Canary = 0x%lx = %lu\n",   stack->leftStackCanary,    stack->leftStackCanary);
    COLOR_PRINT(YELLOW, "Right stack Canary = 0x%lx = %lu\n\n", stack->rightStackCanary,   stack->rightStackCanary);
    )
    ON_STACK_DATA_CANARY
    (
    COLOR_PRINT(YELLOW, "Left  data  Canary = 0x%lx = %lu\n",   GetLeftDataCanary(stack),  GetLeftDataCanary(stack));
    COLOR_PRINT(YELLOW, "Right data  Canary = 0x%lx = %lu\n\n", GetRightDataCanary(stack), GetRightDataCanary(stack));
    )

    ON_STACK_HASH (COLOR_PRINT(BLUE, "stack Hash = %lu\n",   stack->stackHash);)
    ON_STACK_DATA_HASH (COLOR_PRINT(BLUE, "data  Hash = %lu\n\n", stack->dataHash);)

    COLOR_PRINT(CYAN, "size = %lu\n", stack->size);
    COLOR_PRINT(CYAN, "capacity = %lu\n\n", stack->capacity);


    ON_STACK_DATA_POISON (COLOR_PRINT(GREEN, "Poison = 0x%x = %d\n\n", Poison, Poison);)

    COLOR_PRINT(BLUE, "data = \n{\n");
    for (size_t data_i = 0; data_i < stack->size; data_i++)
    {
        COLOR_PRINT(BLUE, "*[%2lu] %d\n", data_i, stack->data[data_i]);
    }

    for (size_t data_i = stack->size; data_i < stack->capacity; data_i++)
    {
        COLOR_PRINT(CYAN, " [%2lu] 0x%x\n", data_i, stack->data[data_i]);   
    }
    COLOR_PRINT(BLUE, "};\n\n");


    COLOR_PRINT(VIOLET, "data ptrs = \n{\n");
    for (size_t data_i = 0; data_i < stack->size; data_i++)
    {
        COLOR_PRINT(VIOLET, "*[%2lu] 0x%p\n", data_i, &stack->data[data_i]);
    }

    for (size_t data_i = stack->size; data_i < stack->capacity; data_i++)
    {
        COLOR_PRINT(CYAN, " [%2lu] 0x%p\n", data_i, &stack->data[data_i]);   
    }
    COLOR_PRINT(VIOLET, "};\n");

    COLOR_PRINT(GREEN, "\n\nDump END\n\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void ErrPlaceCtor (StackErrorType* err, const char* file, int line, const char* func)
{
    assert(err);
    assert(file);
    assert(func);

    err->file = file;
    err->line = line;
    err->func = func;
    return;
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintPlace(const char* file, int line, const char* Function)
{
    COLOR_PRINT(WHITE, "file [%s]\nLine [%d]\nFunc [%s]\n", file, line, Function);
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AssertPrint(StackErrorType err, const char* file, int line, const char* func)
{
    if (err.IsFatalError || err.IsWarning) 
    {
        COLOR_PRINT(RED, "Assert made in:\n");
        PrintPlace(file, line, func);
        PrintError(err);
        ON_STACK_DEBUG
        (
        PrintPlace(err.file, err.line, err.func);
        )
        printf("\n");
    }
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef STACK_VERIF
#undef RETURN_IF_ERR_OR_WARN
