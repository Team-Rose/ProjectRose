using Rose;

using System;

namespace Sandbox
{
    internal class Flap : Entity
    {
        private Rigidbody2DComponent m_RigidBody2D;

        bool m_SpaceWasDown;
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_RigidBody2D = GetComponent<Rigidbody2DComponent>();
        }
        void OnUpdate(float ts)
        {

            if (Input.IsKeyDown(KeyCode.Space))
            {
                if(!m_SpaceWasDown)
                {
                    m_RigidBody2D.ApplyLinearImpulse(new Vector2(0.0f, 3.0f), true);
                }
                m_SpaceWasDown = true;
            } else
            {
                m_SpaceWasDown = false;
            }
                
        }
    }
}

