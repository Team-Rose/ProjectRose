

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
    }

    public class Rigidbody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 point, bool wake)
        {
            InternalCalls.RigidBody2DComponeny_ApplyLinearImpulse(entity.ID, ref impulse, ref point, wake);
        }
        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.RigidBody2DComponeny_ApplyLinearImpulseToCenter(entity.ID, ref impulse, wake);
        }
    }
}
