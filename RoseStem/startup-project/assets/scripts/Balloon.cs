using Rose;

using System;

namespace Sandbox
{
    internal class Balloon : Entity
    {
        public Entity m_Camera;

        public float floatPower = 20.0f;
        public float windPower = 8.0f;

        private Rigidbody2DComponent m_RigidBody2D;

        private float time = 0.0f;

        private bool shouldReset = false;
        private Camera m_CameraObject;
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
            m_CameraObject = m_Camera.As<Camera>();
            Tag = $"{Translation.Y}";
            if (shouldReset)
            {

                m_RigidBody2D.Position = new Vector2(0.0f, 0.0f);
                m_CameraObject.ResetCamera();
                shouldReset = false;
            }

          
            if (Translation.Y - m_Camera.Translation.Y > 2.0f)
                m_CameraObject.cameraSpeed = (Translation.Y - m_Camera.Translation.Y) + m_CameraObject.cameraSpeedBase;
            else
                m_CameraObject.cameraSpeed = m_CameraObject.cameraSpeedBase;

            if (Input.IsKeyDown(KeyCode.Space))
            {
                time += ts * 5;
                if (time > 1.0f)
                {
                    time = 1.0f;
                }

                m_RigidBody2D.ApplyLinearImpulse(new Vector2(0.0f, floatPower * ts), true);
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
                m_RigidBody2D.ApplyLinearImpulse(new Vector2(-windPower * ts, 0.0f), true);
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                m_RigidBody2D.ApplyLinearImpulse(new Vector2(windPower * ts, 0.0f), true);
            }


            float scale = Rose.Math.Lerp(1.0f, 1.1f, time);
            //Console.WriteLine(scale);
            this.Scale = new Vector3(scale);
        }

        private void OnCollision2DBegin(Entity entity)
        {
            if(entity.Name == "Danger")
                shouldReset = true;
        }
    }
}

