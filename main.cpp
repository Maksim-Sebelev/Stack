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
    ASSERT(Ctor(&Stack, DefaultStackSize ON_DEBUG(, __FILE__, __LINE__, __func__, "Stack")));
    
    for (size_t i = 0; i < 10; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i);
        // }
        ASSERT(Push(&Stack, i + 1));
    }

    StackElem_t PopElem = 0;
    for (size_t i = 0; i < 11; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i);
        // }
        ASSERT(Pop(&Stack, &PopElem));
    }


    for (size_t i = 0; i < 4; i++)
    {
        // if (i % 1000 == 0)
        // {
        //     printf("%u\n", i);
        // }
        ASSERT(Push(&Stack, i + 1));
    }

    ASSERT(PrintStack(&Stack));

    ASSERT(Dtor(&Stack));
    return 0;
}
