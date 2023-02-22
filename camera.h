#pragma once
#include "common.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


typedef enum cameraType_e {
	perspective, ortho
}cameraType_e;

class Camera {
private:
	GLFWwindow* window;
	
	glm::vec3 position;
	glm::quat orientation;
	cameraType_e type;

	glm::vec3 worldX = glm::vec3(1, 0, 0);
	glm::vec3 worldY = glm::vec3(0, 1, 0);
	glm::vec3 worldZ = glm::vec3(0, 0, 1);


	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;;
	



	//base camera configuration
	float horizontalAngle = 0;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 90.0f;

	
	//base mouse configuration
	double xpos, ypos;
	int screenx, screeny;
	float xoffset;
	float yoffset;
	float mouseSpeed = 0.1f;

	//roll
	float zoffset;
	float max_roll = 1;
	
	//camera base speed
	float speed = 0.05f;;

	//check awsd qe and space left ctl inputs
	void checkKeys();

	//move "position" by "speed" in the direction of "direction * orientation"
	void move(glm::vec3 direction);

	//rotate quaternion "orientation" by "angle" around "axis"
	void rotate(float angle, glm::vec3 axis);

	void selectCameraType(cameraType_e type);


public:

	Camera(glm::vec3 pos, glm::vec3 lookAt, cameraType_e type);
	void step();
	void computeMatrix();
	glm::vec3 getForwardVector();
	glm::vec3 getPosition();
	glm::mat4 getMatrix();
	glm::mat4 getProjectionMatrix();

	void setWindow(GLFWwindow* window);

};
