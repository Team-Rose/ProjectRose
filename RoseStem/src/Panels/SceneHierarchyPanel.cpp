#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RoseRoot/Scene/Components.h"

#include "../Core/CommandHistory.h"
namespace Rose {
	static std::filesystem::path s_AssetPath;
	static Ref<DeleteEntityCommand> s_DeletedCommand;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{

		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					if (entity.GetComponent<RelationshipComponent>().ParentHandle == 0)
						DrawEntityNode(entity);
				});

			if (s_DeletedCommand != nullptr)
				CommandHistory::Execute(s_DeletedCommand);

			s_DeletedCommand = nullptr;
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			//Right-click on blank
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}
		}

		

		ImGui::End();


		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::SetAssetPath(std::filesystem::path path)
	{
		s_AssetPath = path;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		if (entity.GetComponent<RelationshipComponent>().Children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		
		

		if (ImGui::BeginDragDropSource())
		{
			UUID entityID = entity.GetUUID();
			ImGui::SetDragDropPayload("ENTITY", &entityID, 1 * sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				UUID droppedID = *((UUID*)payload->Data);
				if (Entity droppedEntity = m_Context->GetEntityByUUID(droppedID)) {
					m_Context->ParentEntity(droppedEntity, entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if(ImGui::IsItemClicked())
		{ 
			m_SelectionContext = entity;
		}
		

		bool entityDeleted = false;

		if (Input::IsKeyPressed(Key::Delete) && m_SelectionContext == entity)
			entityDeleted = true;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			if (ImGui::MenuItem("Create Empty Entity")) {
				Entity newEntity = m_Context->CreateEntity("Empty Entity");
				m_Context->ParentEntity(newEntity, entity);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			for (UUID& childID : entity.GetComponent<RelationshipComponent>().Children) {
				if(Entity childEntity = m_Context->GetEntityByUUID(childID))
					DrawEntityNode(childEntity);
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			//TODO only clear selection if the the deleted entity is selelected
			m_SelectionContext = {};
			s_DeletedCommand = CreateRef<DeleteEntityCommand>(m_Context, entity);
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		//TODO Remove
		glm::vec3 tempVec = values;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 0.1f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 0.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue; \
			ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 0.1f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 0.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 0.1f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 0.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight,lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{	
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.HasComponent<LuaScriptComponent>())
			{
				if (ImGui::MenuItem("Lua Script"))
				{
					m_SelectionContext.AddComponent<LuaScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<MonoScriptComponent>())
			{
				if (ImGui::MenuItem("Mono Script"))
				{
					m_SelectionContext.AddComponent<MonoScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			if (!m_SelectionContext.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectionContext.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					m_SelectionContext.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 2D"))
				{
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					m_SelectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				glm::vec3 translation = component.Translation;
				DrawVec3Control("Translation", translation);
				if (translation != component.Translation) {
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(component.Translation, translation));
				}

				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				if (glm::radians(rotation) != component.Rotation) {
					CommandHistory::ChangeVec3(CreateRef < ChangeValueCommand<glm::vec3>>(component.Rotation, glm::radians(rotation)));
				}
				component.Rotation = glm::radians(rotation);

				glm::vec3 scale = component.Scale;
				DrawVec3Control("Scale", scale, 1.0f);
				if (scale != component.Scale) {
					CommandHistory::ChangeVec3(CreateRef<ChangeValueCommand<glm::vec3>>(component.Scale, scale));
				}
			});

		DrawComponent<LuaScriptComponent>("Lua Script", entity, [](auto& component)
			{
				ImGui::Button(component.Path.c_str(), ImVec2(300.0f, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path scriptPath = path;
						component.Path = scriptPath.string();
					}
					ImGui::EndDragDropTarget();
				}
			});
		DrawComponent<MonoScriptComponent>("Mono Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExist = MonoScriptEngine::EntityClassExist(component.ClassName);
				if (!scriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.5f, 0.6f, 1.0f));

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				std::strncpy(buffer, component.ClassName.c_str(), sizeof(buffer));
				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				{
					component.ClassName = std::string(buffer);
					
				}

				//Fields
				UUID id = entity.GetUUID();
				bool sceneIsRunning = scene->IsRunning();

				if (sceneIsRunning) {
					Ref<MonoScriptInstance> scriptInstance = MonoScriptEngine::GetEntityMonoScriptInstance(id);
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetMonoScriptClass()->GetFields();
						for (const auto& [name, field] : fields) {
							if (field.Type == MonoScriptFieldType::Float) {

								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data)) {
									scriptInstance->SetFieldValue<float>(name, data);
								}
							}
						}
					}
				}
				else
				{
					if (scriptClassExist)
					{
						Ref<MonoScriptClass> entityClass = MonoScriptEngine::GetEntityClass(component.ClassName);
						if (entityClass) {
							const auto& fields = entityClass->GetFields();
							auto& entityFields = MonoScriptEngine::GetScriptFieldMap(entity.GetUUID());

							for (const auto& [name, field] : fields) {
								// Field has been set in editor
								if (entityFields.find(name) != entityFields.end()) {
									MonoScriptFieldInstance& scriptFieldInstance = entityFields.at(name);
									if (field.Type == MonoScriptFieldType::Float) {
										float data = scriptFieldInstance.GetValue<float>();
										if (ImGui::DragFloat(name.c_str(), &data)) {
											scriptFieldInstance.SetValue<float>(data);
										}
									}
								}
								else
								{
									// Display control to set it maybe
									if (field.Type == MonoScriptFieldType::Float) {

										//TODO Proper default value (match to C#)
										float data = 0.0f;
										if (ImGui::DragFloat(name.c_str(), &data)) {
											MonoScriptFieldInstance& fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue<float>(data);
										}
									}
								}
							}
						}
					}
				}

				if (!scriptClassExist)
					ImGui::PopStyleColor();
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				//Texture
				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = path;
						component.Path = (s_AssetPath.string() + "\\" + texturePath.string());
						if (Ref<Texture2D> texture = AssetManager::GetOrLoadTexture(component.Path))
							component.Texture = texture;
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 1000.0f);
			});

		DrawComponent<CircleRendererComponent> ("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.0025f, 0.0f, 1.0f);
			});


		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{

				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{

				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Friction",&component.Friction, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{

				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius, 0.5f, 0.0f, 5.0f);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});
	}
}