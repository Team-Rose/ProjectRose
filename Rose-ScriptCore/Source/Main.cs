using System;

namespace Rose
{
    public class Main
    {

        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main constructor!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }

        public void PrintInts(int value, int value2)
        {
            Console.WriteLine($"C# recieved: {value} and {value2}");
        }

        public void PrintString(string str)
        {
            Console.WriteLine($"C# says: {str}");
        }
    }
}
