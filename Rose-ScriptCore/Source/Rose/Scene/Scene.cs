
using Microsoft.Win32;

namespace Rose
{
    public static class Scene
    {
        public static Entity FindEntityByTag(string tag)
        {
            ulong entityID = InternalCalls.Scene_FindEntityByTag(tag);
            Entity newEntity = entityID != 0 ? new Entity(entityID) : null;
            return newEntity;
        }
    }
}
