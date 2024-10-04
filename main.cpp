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
    ErrorType Err = {};

    ASSERT(Ctor(&Stack, DefaultStackSize ON_DEBUG(, __FILE__, __LINE__, __func__, "Stack")));
    Verif(&Stack, &Err);
    ASSERT(Err);

    for (size_t i = 0; i < 1<<4; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i);
        // }
        ASSERT(Push(&Stack, i + 1));
    }


    char* danger = (char*) &Stack;
    for (int i = 32; i < 36; i++)
    {
        danger[i] = 0;
    }
    // DUMP(&Stack);
    // StackElem_t PopElem = 0;
    // for (size_t i = 0; i < 1; i++)
    // {
    //     // if (i % 1000 == 0)
    //     // {
    //     //     printf("%u\n", i); 
    //     // }
    //     ASSERT(Pop(&Stack, &PopElem));
    // }

    // for (size_t i = 0; i < 0; i++)
    // {
    //     // if (i % 1000 == 0)
    //     // {
    //     //     printf("%u\n", i);
    //     // }
    //     ASSERT(Push(&Stack, i + 1));
    // }

    // for (size_t i = 0; i < 3; i++)
    // {
    //     // if (i % 1000 == 0)
    //     // {
    //     //     printf("%u\n", i); 
    //     // }
    //     ASSERT(Pop(&Stack, &PopElem));
    // }
    // DUMP(&Stack);
    ASSERT(PrintStack(&Stack));

    ASSERT(Dtor(&Stack));

    return 0;
}
