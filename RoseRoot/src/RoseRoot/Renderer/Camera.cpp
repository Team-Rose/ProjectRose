#include "rrpch.h"
#include "Camera.h"

namespace Rose {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearClip, float farClip)
		: m_Fov(fov), m_AspectRatio(aspect),m_NearClip(nearClip), m_FarClip(farClip),
		 m_ProjectionMatrix(glm::perspective(glm::radians(fov), m_AspectRatio, nearClip, farClip)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void PerspectiveCamera::RecaculatePojectionMatrix()
	{
		RR_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void PerspectiveCamera::RecaculateViewMatrix()
	{
		RR_PROFILE_FUNCTION();

		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.f, 1.f, 0.f)));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));

		glm::mat4  transform = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		
		m_ViewMatrix = transform;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}



	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f))
	{
		RR_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		RR_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		RR_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}