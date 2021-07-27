#include "hello.h"

int hello(Foo* what)
{
    printf("Hello Wrapper\n");
    printf("what: %p\n", what);
    what->x = 5;
    return what->x;
}