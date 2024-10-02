#include <stdio.h>
#include "Stack.h"
#include "StackFunc.h"
#include "ErrorFunc.h"
#include "ColorPrint.h"
#include "HashFunc.h"

const size_t DefaultStackSize = 2;

int main()
{
    Stack_t Stack = {};
    StackElem_t PopElem = 0;
    ErrorType Err = {};

    // ASSERT(Ctor(&Stack, DefaultStackSize, __FILE__, __LINE__, __func__, "Stack"));
    ASSERT(Ctor(&Stack, DefaultStackSize ON_DEBUG(, __FILE__, __LINE__, __func__, "Stack")));
    Verif(&Stack, &Err);
    ASSERT(Err);


    for (size_t i = 0; i < 1000; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i);
        // }
        ASSERT(Push(&Stack, i + 1));
    }
    for (size_t i = 0; i < 11; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i); 
        // }
        ASSERT(Pop(&Stack, &PopElem));
    }

    // DUMP(&Stack);
    // ASSERT(PrintStack(&Stack));

    ASSERT(Dtor(&Stack));

    return 0;
}