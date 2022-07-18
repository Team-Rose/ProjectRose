using System;
using System.Runtime.CompilerServices;

namespace Rose
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector3(ref Vector3 parameter, out Vector3 result);
    }

    public class Entity
    {

        public float MyPublicFloatVar = 5.0f;

        private string m_Name = "Hello";
        public string Name
        {
            get => m_Name;
            set
            {
                m_Name = value;
                MyPublicFloatVar += 5.0f;
            }
        }

        public Entity()
        {
            Console.WriteLine("Main constructor!");
            Log("Rose is Swag", 34980);

            Vector3 pos = new Vector3(2, 4, 3);
            Vector3 result = Log(pos);
            Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");
        }

        public void PrintMessage()
        {
            Console.WriteLine($"C# Says: m_Name = {m_Name}, MyPublicFloatVar = {MyPublicFloatVar}");
        }

        public void PrintInts(int value, int value2)
        {
            Console.WriteLine($"C# recieved: {value} and {value2}");

        }

        public void PrintString(string str)
        {
            Console.WriteLine($"C# says: {str}");
        }

        public void Log(string text, int parameter)
        {
            InternalCalls.NativeLog(text, parameter);
        }

        public Vector3 Log(Vector3 parameter)
        {
            InternalCalls.NativeLog_Vector3(ref parameter, out Vector3 result);
            return result;
        }


    }
}
