#pragma once;

//Translation values
enum Movement {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	UP,
	DOWN
};

//Default Camera values
const GLfloat SPEED = 0.99f;
const GLfloat SENSITIVITY = 0.002f;

class Camera{

	//attribute vectors
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	//reference variable
	glm::vec3 worldUp;

	//camera translation variable
	GLfloat speed;

	//camera rotation variables
	GLfloat sensitivity;
	GLfloat yaw;
	GLfloat pitch;

public:
	//Constructor: default has camera positioned at the origin looking down the -z axis
	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	
	//Applies the glm lookAt function to define the View Matrix
	glm::mat4 getViewMatrix();

	//Adjusts the camera's position
	void translateCamera(Movement direction);

	//Adjusts the camera's direction of view
	void rotateCamera(GLfloat xOffset, GLfloat yOffset);

	void climbAt(float y);

	//Current Position of camera
	glm::vec3 getPosition() { return position; };
	glm::vec3 getFront() { return forward; };

	glm::vec3 getNextPosition() {
		glm::vec3 nextPos(this->forward.x * speed, 0, this->forward.z * speed);
		return nextPos;
	};

private:
	//Recalculates all attribute vectors to reflect movement changes
	void recalculateVectors();

};

