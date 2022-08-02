using Rose;

using System;

namespace Sandbox
{
    internal class Flap : Entity
    {
        private Rigidbody2DComponent m_RigidBody2D;
        private TransformComponent m_Transform;

        bool m_SpaceWasDown;
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_RigidBody2D = GetComponent<Rigidbody2DComponent>();
            m_Transform = GetComponent<TransformComponent>();
        }
        void OnUpdate(float ts)
        {

            if (Input.IsKeyDown(KeyCode.Space))
            {
                m_Transform.Scale = new Vector3(1.1f);

                m_RigidBody2D.ApplyLinearImpulse(new Vector2(0.0f, 20.0f*ts), true);

                m_SpaceWasDown = true;
            } else
            {
                m_Transform.Scale = new Vector3(1.0f);
                m_SpaceWasDown = false;
            }
                
        }
    }
}

