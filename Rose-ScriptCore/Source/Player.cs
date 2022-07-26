using Rose;

using System;

namespace Sandbox
{
    internal class Player : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
        }
        void OnUpdate(float ts)
        {
            //Console.WriteLine($"Player.OnUpdate: {ts}");

            Vector3 vel = Vector3.Zero;
            float speed = 4.0f;

            if (Input.IsKeyDown(KeyCode.Up))
                vel.Y += 1.0f;
            if (Input.IsKeyDown(KeyCode.Down))
                vel.Y += -1.0f;

            if (Input.IsKeyDown(KeyCode.Left))
                vel.X += -1.0f;
            if (Input.IsKeyDown(KeyCode.Right))
                vel.X += 1.0f;

            Translation += (vel * speed) * ts;
        }
    }
}
