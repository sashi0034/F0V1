#include "pch.h"

#include "EntryPoint_AS.h"
#include "EntryPoint_CPP.h"

void Main()
{
#if 1
    EntryPoint_CPP();
#else
    EntryPoint_AS();
#endif
}
