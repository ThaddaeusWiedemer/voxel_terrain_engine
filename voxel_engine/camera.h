#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum direction{
    FORWARD = 0,
    BACKWARD,
    LEFT,
    RIGHT,
	UP,
	DOWN
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat TEST      =  3.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera{
private:
	glm::mat4 viewMatrix;
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	// Euler Angles
	GLfloat yaw;
	GLfloat pitch;
	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void UpdateCameraVectors(){
		// Calculate new _front vector
		glm::vec3 _front;
        _front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        _front.y = sin(glm::radians(this->pitch));
        _front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(_front);
		// Re-calculate right and up vector
        // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in
        // slower movement.
		this->right = glm::normalize(glm::cross(this->front, this->world_up));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

public:
    // Constructor with vectors
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH)
            : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(TEST), mouseSensitivity(SENSITIVTY),
            zoom(ZOOM){
		this->viewMatrix = glm::mat4(1.f);

		this->position = position;

        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;

        this->UpdateCameraVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(TEST), mouseSensitivity(SENSITIVTY), zoom(ZOOM){
		this->position = glm::vec3(posX, posY, posZ);
        this->world_up = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->UpdateCameraVectors();
    }

	~Camera()= default;

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix(){
		this->UpdateCameraVectors();
		this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
		return this->viewMatrix;
    }

	glm::vec3 getPosition(){
		return this->position;
	}

	glm::mat4 getProjectionMatrix(GLfloat width, GLfloat height){
		return glm::perspective(glm::radians(this->zoom), width / height, 0.1f, 1000.0f);
	}

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera
    // defined ENUM (to abstract it from windowing systems)
    void Move(direction direction, GLfloat dt){
        GLfloat velocity = this->movementSpeed * dt;
		switch(direction){
		case FORWARD:
			this->position += this->front * velocity;
			break;
		case BACKWARD:
			this->position -= this->front * velocity;
			break;
		case LEFT:
			this->position -= this->right * velocity;
			break;
		case RIGHT:
			this->position += this->right * velocity;
			break;
		case UP:
			this->position += this->up * velocity;
			break;
		case DOWN:
			this->position -= this->up * velocity;
			break;
		default:
			break;
		}
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrainPitch = true){
        x_offset *= this->mouseSensitivity;
        y_offset *= this->mouseSensitivity;

        this->yaw   += x_offset;
        this->pitch += y_offset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch){
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        this->UpdateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat y_offset){
		float min = 0.1f;
		float max = 200.f;
        if (this->zoom >= min && this->zoom <= max)
            this->zoom -= y_offset;
        if (this->zoom <= min)
            this->zoom = min;
        if (this->zoom >= max)
            this->zoom = max;
    }

	//
	void invert(){
		this->pitch *= -1;
		this->position *= glm::vec3(1.f, -1.f, 1.f);
	}
};
