#pragma once

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 6.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 rt;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;




public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) :
		front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
		front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
		
	}

	glm::vec3 getPosition() { return this->position; }

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->movementSpeed * deltaTime;
		if (FORWARD == direction)
		{
			this->position += this->front * velocity;
		}

		if (BACKWARD == direction)
		{
			this->position -= this->front * velocity;
		}

		if (LEFT == direction)
		{
			this->position -= this->rt * velocity;
		}

		if (RIGHT == direction)
		{
			this->position += this->rt * velocity;
		}
	}

	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;
		
		this->yaw += xOffset;
		this->pitch += yOffset;

		if (constrainPitch)
		{
			if (this->pitch > 89.0f)
			{
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f)
			{
				this->pitch = -89.0f;
			}
		}

		this->updateCameraVectors();
	}

	void ProcessMouseScroll(GLfloat yOffset)
	{
		if (this->zoom >= 1.0f && this->zoom < -45.0f)
		{
			this->zoom -= yOffset;
		}

		if (this->zoom <= 1.0f)
		{
			this->zoom = 1.0f;
		}
		if (this->zoom >= 45.0f)
		{
			this->zoom = 45.0f;
		}
	}

	GLfloat GetZoom()
	{
		return this->zoom;
	}



	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(front);
		this->rt = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->rt, this->front));

	}


};

