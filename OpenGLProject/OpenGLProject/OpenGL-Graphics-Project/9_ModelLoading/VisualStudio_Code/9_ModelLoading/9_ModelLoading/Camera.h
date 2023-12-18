#ifndef CAMERA_H
#define CAMERA_H

const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	//Camera transform
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	//----------------

	//Camera parameters
	float movementSpeed;
	float rotationSpeed;
	float fov;
	float pitch;
	float yaw;
	//-----------------

	Camera(glm::vec3 startingPosition, glm::vec3 startingForward)
	{
		position = startingPosition;
		forward = startingForward;

		pitch = 0.0f;
		yaw = -90.0f;

		movementSpeed = 2.0f;
		rotationSpeed = 1.0f;

		fov = 45.0f; //fov = zoom

		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + forward, up);
	}

	void HandleKeyboard(CameraMovement direction, float deltaTime)
	{
		float finalSpeed = movementSpeed * deltaTime;

		if (direction == FORWARD)
			position += forward * finalSpeed;
		if (direction == BACKWARD)
			position -= forward * finalSpeed;

		if (direction == LEFT)
			position -= right * finalSpeed;
		if (direction == RIGHT)
			position += right * finalSpeed;

		if (direction == UP)
			position += up * finalSpeed;
		if (direction == DOWN)
			position -= up * finalSpeed;
	}

	void HandleMouseMovement(float xOffset, float yOffset, float deltaTime)
	{
		float finalSpeed = rotationSpeed * deltaTime;

		yaw += xOffset * finalSpeed;
		pitch -= yOffset * finalSpeed;
		if (pitch < -90.0f)
			pitch = -90.0f;
		if (pitch > 90.0f)
			pitch = 90.0f;

		UpdateCameraVectors();
	}

	void HandleMouseScroll(float offset)
	{
		fov -= offset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 90.0f)
			fov = 90.0f;
	}
private:
	void UpdateCameraVectors()
	{
		glm::vec3 direction;
		direction.x = glm::cos(glm::radians(yaw));
		direction.y = glm::sin(glm::radians(pitch));
		direction.z = glm::sin(glm::radians(yaw));

		forward = glm::normalize(direction);
		right = glm::normalize(glm::cross(forward, WORLD_UP)); // works because we do not have ROLL
		up = glm::normalize(glm::cross(right, forward));
	}
};

#endif