using System;
using System.Collections.Generic;


namespace Rose
{
    //TODO Consider changing the name of this calls to make it less ambiguous between system and rose math
    public static class Math
    {
        static public float Lerp(float min, float max, float time)
        {
            return (1 - time) * min + time * max;
        }
    }
}
