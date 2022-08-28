using Rose;

using System;

namespace Sandbox
{
    internal class Player : Entity
    {
        private TransformComponent m_Transfrom;
        protected override void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transfrom = GetComponent<TransformComponent>();
        }
        protected override void OnUpdate(float ts)
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

            m_Transfrom.Translation += (vel * speed) * ts;
        }
    }
}
