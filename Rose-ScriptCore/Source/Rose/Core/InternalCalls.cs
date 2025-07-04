﻿using System;
using System.Runtime.CompilerServices;

namespace Rose
{
    internal static class InternalCalls
    {
        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);
        #endregion

        #region Asset
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Asset_GetOrLoadTexture2D(string path);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Asset_GetTexture2D(string path);
        #endregion

        #region Scene
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_FindEntityByTag(string tag);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_CreateEntity(string tag);
        #endregion

        #region Entity
        //Relationship management
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_GetParent(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetParent(ulong entityID, ulong parentID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong[] Entity_GetChildren(ulong entityID);

        //Component
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveComponent(ulong entityID, Type componentType);

        //Tag
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Entity_GetTag(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTag(ulong entityID, string tag);

        //Object
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object Entity_GetScriptInstance(ulong entityID);
        
        #endregion

        #region TransformComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityID, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);
        #endregion

        #region SpriteRendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SpriteRendererComponent_GetTileFactor (ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTileFactor(ulong entityID, float tileFactor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTexture2D(ulong entityID, string path);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string SpriteRendererComponent_GetTexture2D(ulong entityID);
        #endregion

        #region RigidBody2DComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_GetPosition(ulong entityID, out Vector2 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetPosition(ulong entityID, ref Vector2 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody2DComponent_GetRotation(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetRotation(ulong entityID, float rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
        #endregion
    }
}
