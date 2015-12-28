extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}


extern "C"
{
    // A function adding two integers and returning the result
    int SampleAddInt(int i1, int i2)
    {
        return i1 + i2;
    }

    // A function doing nothing ;)
    void SampleFunction1()
    {
        // insert code here
    }

    // A function always returning zero
    int SampleFunction2()
    {
        // insert code here

        return 0;
    }
}
