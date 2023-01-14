
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
		public static Entity FindEntityByName(string name) { return FindEntityByTag(name); }

		public static Entity CreateEntity(string tag)
		{
			ulong entityID = InternalCalls.Scene_CreateEntity(tag);
			Entity newEntity = entityID != 0 ? new Entity(entityID) : null;
			return newEntity;
		}
	}
}
