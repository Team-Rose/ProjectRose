

namespace Rose
{
    public abstract class Component
    {
        public Entity entity { get; internal set; }
    }
    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(entity.ID, ref value);
            }
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(entity.ID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(entity.ID, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(entity.ID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(entity.ID, ref value);
            }
        }
    }

    public class Rigidbody2DComponent : Component
    {
        public void SetPosition(Vector2 position)
        {
            InternalCalls.RigidBody2DComponent_SetPosition(entity.ID, ref position);
        }
        public Vector2 GetPosition()
        {
            InternalCalls.RigidBody2DComponent_GetPosition(entity.ID, out Vector2 position);
            return position;
        }
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 point, bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(entity.ID, ref impulse, ref point, wake);
        }
        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCenter(entity.ID, ref impulse, wake);
        }
        
    }
}
