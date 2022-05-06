#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rose {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fov, float aspect, float nearClip, float farClip);

		const float& GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(const float& aspect) { m_AspectRatio = aspect; RecaculatePojectionMatrix(); }

		const float& GetFOV() const { return m_Fov; }
		void SetFOV(const float& fov) { m_Fov = fov; RecaculatePojectionMatrix(); }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecaculateViewMatrix(); }

		const float& GetYaw() const { return m_Yaw; }
		void SetYaw(const float& yaw) { m_Yaw = yaw; RecaculateViewMatrix(); }

		const float& GetPitch() const { return m_Pitch; }
		void SetPitch(const float& pitch) { m_Pitch = pitch; RecaculateViewMatrix(); }

		const glm::vec3& GetFront() const { return m_Front; }
		const glm::vec3& GetUp() const { return m_Up; }
		const glm::vec3& GetRight() const { return m_Right; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecaculateViewMatrix();
		void RecaculatePojectionMatrix();
	private:
		float m_Fov;
		float m_AspectRatio;
		float m_NearClip;
		float m_FarClip;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.f,0.f,0.f };
		glm::vec3 m_Direction = { 0.f,0.f,0.f };

		glm::vec3 m_Front = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_Right = glm::vec3(0.0f, 0.0f, 1.0f);

		float m_Yaw = 0.f;
		float m_Pitch = 0.f;
	};

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};
}