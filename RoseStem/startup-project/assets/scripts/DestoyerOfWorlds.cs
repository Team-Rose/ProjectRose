using Rose;

using System;

namespace Sandbox
{
    internal class DestoyerOfWorlds : Entity
    {
        public float m_Power = 20.0f;

        private Rigidbody2DComponent m_RigidBody2D;
        protected override void OnCreate()
        {
            Console.WriteLine($"DestroyerOfWorlds.OnCreate - {Tag} - {ID}");

            m_RigidBody2D = GetComponent<Rigidbody2DComponent>();
        }
        protected override void OnUpdate(float ts)
        {
            Vector2 impulseDir = new Vector2(0.0f, 0.0f);
            if (Input.IsKeyDown(KeyCode.A))
            {
                impulseDir.X += -1.0f;
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                impulseDir.X += 1.0f;
            }
            if (Input.IsKeyDown(KeyCode.W))
            {
                impulseDir.Y += 1.0f;
            }
            if (Input.IsKeyDown(KeyCode.S))
            {
                impulseDir.Y += -1.0f;
            }
            //TODO Normalize
            m_RigidBody2D.ApplyLinearImpulse(impulseDir * m_Power, true);
        }
    }
}

