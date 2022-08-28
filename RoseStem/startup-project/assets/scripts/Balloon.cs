using Rose;

using System;

namespace Sandbox
{
    internal class Balloon : Entity
    {
        private Rigidbody2DComponent m_RigidBody2D;
        private TransformComponent m_Transform;

        private float time = 0.0f;


        protected override void OnCreate()
        {
            Console.WriteLine($"Balloon.OnCreate - {ID}");

            m_RigidBody2D = GetComponent<Rigidbody2DComponent>();
            m_Transform = GetComponent<TransformComponent>();
        }
        protected override void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.Space))
            {
                time += ts * 5;
                if (time > 1.0f)
                {
                    time = 1.0f;
                }

                m_RigidBody2D.ApplyLinearImpulse(new Vector2(0.0f, 19.0f * ts), true);
            }
            else
            {
                time -= ts * 5;
                if (time < 0.0f)
                {
                    time = 0.0f;
                }
            }

            if (Input.IsKeyDown(KeyCode.A))
            {
                m_RigidBody2D.ApplyLinearImpulse(new Vector2(-8.0f * ts, 0.0f), true);
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                m_RigidBody2D.ApplyLinearImpulse(new Vector2(8.0f * ts, 0.0f), true);
            }

            float scale = Rose.Math.Lerp(1.0f, 1.1f, time);
            //Console.WriteLine(scale);
            m_Transform.Scale = new Vector3(scale);

        }
    }
}

