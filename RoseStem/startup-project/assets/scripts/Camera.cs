using Rose;

using System;

namespace Sandbox
{
    internal class Camera : Entity
    {
        private TransformComponent m_Transform;

        private float time = 0.0f;

        protected override void OnCreate()
        {
            Console.WriteLine($"Camera.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
        }
        protected override void OnUpdate(float ts)
        {
            m_Transform.Translation += new Vector3(0.0f, 1.0f*ts, 0.0f);
        }
    }
}

