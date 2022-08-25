#pragma once
#include "RoseRoot.h"

namespace Rose {
	//Commands are store back to front and when the commandBuffer goes above the max size the back one gets deleted

	class Command {
	public:
		virtual ~Command() = default;

		virtual void Execute() = 0;
		virtual void Undo() = 0;
		virtual void Lock() = 0;
		virtual bool IsLocked() = 0;
	};

	class DeleteEntityCommand : public Command {
	public:
		DeleteEntityCommand(const Ref<Scene>& editorScene, Entity entity)
		: m_Context(editorScene), m_DeletedEntity(entity)
		{
			m_Scene = CreateRef<Scene>();
			m_CopyEntity = m_Scene->DuplicateEntity(m_DeletedEntity);
		}
		virtual ~DeleteEntityCommand() = default;

		virtual void Execute() override {
			m_Context->DestroyEntity(m_DeletedEntity);
		}
		virtual void Undo() override {
			m_Context->DuplicateEntity(m_CopyEntity);
		}
		virtual void Lock() override {}
		virtual bool IsLocked() override { return true; }
	private:
		Entity m_DeletedEntity;
		Entity m_CopyEntity;
		Ref<Scene> m_Scene;
		Ref<Scene> m_Context;
	};

	template<typename T>
	class ChangeValueCommand : public Command{
	public:
		ChangeValueCommand(T& valToChange, T newVal)
			: m_ValToChange(valToChange), m_NewVal(newVal) {}
		virtual ~ChangeValueCommand() = default;

		virtual void Execute() override {
			m_OldVal = m_ValToChange;
			m_ValToChange = m_NewVal;
		}
		virtual void Undo() override {
			m_ValToChange = m_OldVal;
		}
		virtual void Lock() override { m_Locked = true; }
		virtual bool IsLocked() override { return m_Locked; }

		void Ammend(Ref<ChangeValueCommand<T>> command) {
			m_NewVal = command->getNewVal();
			m_ValToChange = m_NewVal;
		}

		T getNewVal() { return m_NewVal; }
		T& getPointer() { return m_ValToChange; }
	private:
		bool m_Locked = false;
		T& m_ValToChange;
		T m_NewVal;
		T m_OldVal;
	};
	
	class CommandHistory {
	public:
		static void Init();
		static void Clear();

		//A generic execute for command without ammending
		static void Execute(Ref<Command> command);
		
		static void Undo();
		static void Redo();
		//Makes the last command not ammendable
		static void LockLastCommand();

		//Special cases for ammendable commands
		static void ChangeVec3(Ref<ChangeValueCommand<glm::vec3>> command);
		
	};
}