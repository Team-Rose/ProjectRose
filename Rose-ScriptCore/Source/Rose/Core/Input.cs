﻿namespace Rose
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }
        public static bool IsKeyUp(KeyCode keycode)
        {
            return !InternalCalls.Input_IsKeyDown(keycode);
        }
    }
}
