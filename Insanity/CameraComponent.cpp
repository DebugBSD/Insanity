#include "CameraComponent.h"

CameraComponent::CameraComponent(const glm::vec3& pos, const glm::vec3& up, const GLfloat yaw, const GLfloat pitch, const float movementSpeed, const float turnSpeed):
	m_Position{pos},
	m_Yaw{ yaw },
	m_Pitch{ pitch },
	m_Front{glm::vec3{0.0f, 0.0f, -1.0f}}, // Front in OpenGL is in negative direction.
	m_MovementSpeed{movementSpeed},
	m_TurnSpeed{turnSpeed},
	m_WorldUp{ up }
{
	Update();
}

void CameraComponent::KeyControl(const bool* keys, GLfloat dt)
{
	GLfloat t = m_MovementSpeed * dt;

	if (keys[GLFW_KEY_W])
	{
		m_Position += m_Front * t;
	}

	if (keys[GLFW_KEY_S])
	{
		m_Position -= m_Front * t;
	}

	if (keys[GLFW_KEY_D])
	{
		m_Position += m_Right * t;
	}

	if (keys[GLFW_KEY_A])
	{
		m_Position -= m_Right * t;
	}

}

void CameraComponent::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= m_TurnSpeed;
	yChange *= m_TurnSpeed;

	m_Yaw += xChange;
	m_Pitch += yChange;

	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;

	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	Update();
}

const glm::mat4& CameraComponent::CalculateViewMatrix()
{
	return glm::lookAt(m_Position, m_Position+m_Front, m_Up);

}

CameraComponent::~CameraComponent()
{
	
}

void CameraComponent::Update()
{
	m_Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front.y = sin(glm::radians(m_Pitch));
	m_Front.z = sin(glm::radians(m_Yaw))* cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(m_Front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
