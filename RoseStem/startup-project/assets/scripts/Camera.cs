using Rose;

using System;

namespace Sandbox
{
    internal class Camera : Entity
    {
        public float cameraSpeedBase = 1.0f;
        public float cameraSpeed = 0.0f;

        protected override void OnCreate()
        {
            Console.WriteLine($"Camera.OnCreate - {ID}");

        }
        protected override void OnUpdate(float ts)
        {
            Translation += new Vector3(0.0f, cameraSpeed * ts, 0.0f);
        }

        public void ResetCamera()
        {
            Translation = new Vector3(0.0f, 0.0f, 0.0f);
        }
    }
}