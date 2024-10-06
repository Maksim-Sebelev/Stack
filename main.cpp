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
    StackElem_t PopELem = 0;
    ASSERT(Ctor(&Stack, DefaultStackSize ON_DEBUG(, __FILE__, __LINE__, __func__, "Stack")));
    ASSERT(Push(&Stack, 0));
    ASSERT(Pop(&Stack, &PopELem));
    ASSERT(PrintStack(&Stack));
    ASSERT(Dtor(&Stack));
    return 0;
}
