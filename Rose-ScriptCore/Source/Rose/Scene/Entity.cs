using System;

namespace Rose
{

	public class Entity
	{
		public event Action<Entity> Collision2DBeginEvent;
		public event Action<Entity> Collision2DEndEvent;

		protected Entity() { ID = 0; }
		internal Entity(ulong id)
		{
			ID = id;
		}

		public readonly ulong ID;

		protected virtual void OnCreate() { }
		protected virtual void OnUpdate(float ts) { }

		private void OnCollision2DBeginInternal(ulong id) => Collision2DBeginEvent?.Invoke(new Entity(id));
		private void OnCollision2DEndInternal(ulong id) => Collision2DEndEvent?.Invoke(new Entity(id));

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);

		}
		public T GetComponent<T>() where T : Component, new()
		{
			if(!HasComponent<T>())
				return null;

			return new T() { entity = this };
		}
		public T AddComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			Type componentType = typeof(T);
			InternalCalls.Entity_AddComponent(ID, componentType);
			return new T() { entity = this };
		}

		public void RemoveComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return;
			Type componentType = typeof(T);
			InternalCalls.Entity_RemoveComponent(ID, componentType);
		}
		public T As<T>() where T : Entity, new()
		{
			//TODO make sure entity has a valid script component
			object instance = InternalCalls.Entity_GetScriptInstance(ID);
			return instance as T;
		}

		public string Tag
		{
			get
			{
				return InternalCalls.Entity_GetTag(ID);
			}
			set
			{
				InternalCalls.Entity_SetTag(ID, value);
			}
		}
		public string Name
		{
			get
			{
				return InternalCalls.Entity_GetTag(ID);
			}
			set
			{
				InternalCalls.Entity_SetTag(ID, value);
			}
		}

		public Entity Parent
		{
			get
			{
				return new Entity(InternalCalls.Entity_GetParent(ID));
			}
			set
			{
				InternalCalls.Entity_SetParent(ID, value.ID);
			}
		}
		public Entity[] Children
		{
			get
			{
				ulong[] Ids = InternalCalls.Entity_GetChildren(ID);
				Entity[] Entities = new Entity[Ids.Length];

				for(int i = 0; i < Ids.Length; i++)
				{
					Entities[i] = new Entity(Ids[i]);
				}
				return Entities;
			}
		}

		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 translation);
				return translation;
			}
			set
			{
				InternalCalls.TransformComponent_SetTranslation(ID, ref value);
			}
		}
		public Vector3 Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(ID, out Vector3 rotation);
				return rotation;
			}
			set
			{
				InternalCalls.TransformComponent_SetRotation(ID, ref value);
			}
		}
		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(ID, out Vector3 scale);
				return scale;
			}
			set
			{
				InternalCalls.TransformComponent_SetScale(ID, ref value);
			}
		}
	}
}
