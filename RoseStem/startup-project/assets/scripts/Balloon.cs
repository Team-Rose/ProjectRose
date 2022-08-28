using Rose;

using System;

namespace Sandbox
{
    internal class Balloon : Entity
    {
        private Entity m_Camera;

        private Rigidbody2DComponent m_RigidBody2D;

        private float time = 0.0f;
        private bool shouldReset = false;
        protected override void OnCreate()
        {
            Console.WriteLine($"Balloon.OnCreate - {Tag} - {ID}");
             
            m_RigidBody2D = GetComponent<Rigidbody2DComponent>();

            m_Camera = Scene.FindEntityByTag("Camera");
            if (m_Camera == null)
                Console.WriteLine("Camera not found");

            Collision2DBeginEvent += OnCollision2DBegin;
        }
        protected override void OnUpdate(float ts)
        {
            Tag = $"{Translation.Y}";
            if(shouldReset)
            {

                m_RigidBody2D.SetPosition(new Vector2(0.0f, 0.0f));
                if (m_Camera != null)
                    m_Camera.Translation = new Vector3(0.0f, Translation.Y, 0.0f);
                shouldReset = false;
            }

            if (Translation.Y - m_Camera.Translation.Y > 3.0f)
                m_Camera.Translation += new Vector3(0.0f, (Translation.Y - m_Camera.Translation.Y) * ts, 0.0f);

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
            this.Scale = new Vector3(scale);
        }

        private void OnCollision2DBegin(Entity entity)
        {
            shouldReset = true;
        }
    }
}

