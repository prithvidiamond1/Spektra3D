#include "Camera.h"

void Camera::updateCamVectors()
{
	glm::vec3 Front;	// not normalized
	Front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	Front.y = sin(glm::radians(this->pitch));
	Front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front = glm::normalize(Front);	// normalized
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->movementSpeed = SPEED;
	this->mouseSensitivity = SENSITIVITY;
	this->fov = FOV;
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->updateCamVectors();
}

glm::mat4 Camera::getViewMat()
{
	return glm::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::processKeyboardInput(CamMovement direction, float deltaTime)
{
	float velocity = this->movementSpeed * deltaTime;
	if (direction == CamMovement::FORWARD) {
		this->position += this->front * velocity;
	}

	if (direction == CamMovement::BACKWARD) {
		this->position -= this->front * velocity;
	}

	if (direction == CamMovement::LEFT) {
		this->position -= this->right * velocity;
	}

	if (direction == CamMovement::RIGHT) {
		this->position += this->right * velocity;
	}
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= this->mouseSensitivity;
	yOffset *= this->mouseSensitivity;

	this->yaw += xOffset;
	this->pitch += yOffset;

	if (constrainPitch) {
		if (this->pitch > 89.0f) {
			this->pitch = 89.0f;
		}
		if (this->pitch < -89.0f) {
			this->pitch = -89.0f;
		}
	}

	this->updateCamVectors();
}

void Camera::processMouseScroll(float offset)
{
	this->fov -= offset;
	if (this->fov < 1.0f) {
		this->fov = 1.0f;
	} 
	if (this->fov > 45.0f) {
		this->fov = 45.0f;
	}
}

