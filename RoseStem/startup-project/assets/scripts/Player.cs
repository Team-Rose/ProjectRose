using Rose;

using System;

namespace Sandbox
{
    internal class Player : Entity
    {
        private TransformComponent m_Transfrom;
        private SpriteRendererComponent m_Sprite;
        protected override void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transfrom = GetComponent<TransformComponent>();

            m_Sprite = GetComponent<SpriteRendererComponent>();

            m_Sprite.Texture = new Texture2D("textures/Stone");
            if (m_Sprite.Texture == null)
                Console.WriteLine("Oh no the texture isn't valid");

            m_Sprite.Texture = new Texture2D("textures/Stone.png");
            if (m_Sprite.Texture != null)
                Console.WriteLine("Hey it works!");

            m_Sprite.Color = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            m_Sprite.TileFactor = 1.0f;
        }
        protected override void OnUpdate(float ts)
        {
            
        }
    }
}
