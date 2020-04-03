#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class CameraComponent
{
public:
	CameraComponent(const glm::vec3 &pos, const glm::vec3& up, const GLfloat yaw, const GLfloat pitch, const float movementSpeed, const float turnSpeed);

	void KeyControl(const bool*keys, GLfloat dt);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	const glm::mat4& CalculateViewMatrix();

	~CameraComponent();
private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	GLfloat m_Yaw;
	GLfloat m_Pitch;

	GLfloat m_MovementSpeed;
	GLfloat m_TurnSpeed;

	void Update();
};

