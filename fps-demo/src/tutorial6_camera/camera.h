#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW            = -90.0f;
const float PITCH          =   0.0f;
const float MOVEMENT_SPEED =   7.0f;
const float SPRINT_SPEED   =  14.0f;
const float SENSITIVITY    =   0.05f;
const float ZOOM           =  45.0f;

const float BOUNDARY_X1 = -8.5f;
const float BOUNDARY_X2 =  8.5f;
const float BOUNDARY_Z1 = -8.5f;
const float BOUNDARY_Z2 =  8.5f;

const float GRAVITY = 14.0f;
const float JUMP_INITIAL_VELOCITY = 7.0f;

bool sprintEnabled = false;
bool isInAir = false;
float yVelocity = 0.0f;

class Camera
{
public:
	// camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float SprintSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Camera(glm::vec3 position =
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0f),
		float yaw = YAW,
		float pitch = PITCH)
		:
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(MOVEMENT_SPEED),
		SprintSpeed(SPRINT_SPEED),
		MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(
		float posX, float posY, float posZ,
		float upX, float upY, float upZ,
		float yaw, float pitch)
		:
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(MOVEMENT_SPEED),
		SprintSpeed(SPRINT_SPEED),
		MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		//return glm::lookAt(Position, Position + Front, Up);
		return calculate_lookAt_matrix(Position, Position + Front, Up);
	}

	void SetSprint(bool isTrue)
	{	
		sprintEnabled = isTrue;
	}

	void Jump()
	{
		if (!isInAir)
		{
			isInAir = true;
			yVelocity = JUMP_INITIAL_VELOCITY;
			//std::cout << "Camera.Jump()" << std::endl;
		}
	}
	
	void Update(float deltaTime)
	{
		if (isInAir)
		{
			//std::cout << "Camera.Update() isInAir==true" << std::endl;
			Position.y += yVelocity * deltaTime;
			yVelocity -= GRAVITY * deltaTime;
		}
		if (Position.y < 2.0f) //@Change make variable for this in player class
		{
			Position.y = 2.0f;
			isInAir = false;
		}
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity;
		if (sprintEnabled)
			velocity = SprintSpeed * deltaTime;
		else
			velocity = MovementSpeed * deltaTime;

		if (direction == FORWARD)
			Position += GetXZDirection(Front) * velocity;
		if (direction == BACKWARD)
			Position -= GetXZDirection(Front) * velocity;
		if (direction == LEFT)
			Position -= GetXZDirection(Right) * velocity;
		if (direction == RIGHT)
			Position += GetXZDirection(Right) * velocity;


		// Check boundaries
		if (Position.x < BOUNDARY_X1)
			Position.x = BOUNDARY_X1;
		if (Position.x > BOUNDARY_X2)
			Position.x = BOUNDARY_X2;
		if (Position.z < BOUNDARY_Z1)
			Position.z = BOUNDARY_Z1;
		if (Position.z > BOUNDARY_Z2)
			Position.z = BOUNDARY_Z2;
	}

	glm::vec3 GetXZDirection(glm::vec3 front)
	{
		front.y = 0.0f;
		return glm::normalize(front);
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float) yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}

	glm::mat4 calculate_lookAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
	{
		// 1. Position = known
		// 2. Calculate cameraDirection
		glm::vec3 zaxis = glm::normalize(position - target);
		// 3. Get positive right axis vector
		glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(worldUp), zaxis));
		// 4. Calculate camera up vector
		glm::vec3 yaxis = glm::cross(zaxis, xaxis);

		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
		translation[3][0] = -position.x; // Third column, first row
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0][0] = xaxis.x; // First column, first row
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;
		rotation[0][1] = yaxis.x; // First column, second row
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;
		rotation[0][2] = zaxis.x; // First column, third row
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return rotation * translation; // Remember to read from right to left (first translation then rotation)
	}

};
#endif