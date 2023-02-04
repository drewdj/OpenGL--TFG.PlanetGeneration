#include "camera.h"
#include "inputManager.h"
#include <iostream>
#include <time.h>



Camera::Camera(glm::vec3 pos, glm::vec3 lookAt, cameraType_e type)
{

    this->position = pos;
    this->orientation = glm::quat(lookAt);
    
   
    this->viewMatrix = glm::mat4(1.0f);
    this->type = type;
	this->zoffset;
	this->max_roll;

	selectCameraType(type);


}

void Camera::computeMatrix() {

	glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);    
	this->viewMatrix = glm::mat4_cast(orientation) * translate;

}

void Camera::checkKeys() {
    glm::vec3 worldOffset;
    
    if (InputManager::keys['Q'])
    {
        zoffset = - max_roll;

    }

    if (InputManager::keys['E'])
    {
        zoffset = + max_roll;

    }

    if (InputManager::keys['W'])
    {
        move(worldZ);
    }

    if (InputManager::keys['S'])
    {
        move(-worldZ);
    }

    if (InputManager::keys['A'])
    {
		move(worldX);
    }

    if (InputManager::keys['D'])
    {

		move(-worldX);
    }

    if (InputManager::keys[' '])
    {
		move(-worldY);
    }

    if (InputManager::keys[GLFW_KEY_LEFT_CONTROL])
    {
		move(worldY);
    }

	//if arrow up increase speed
	if (InputManager::keys[GLFW_KEY_UP])
	{
		speed = speed + 0.0001f;
	}
    
	//if arrow down decrease speed

	if (InputManager::keys[GLFW_KEY_DOWN])
	{
		speed = speed - 0.0001f;
	}
    

}

void Camera::rotate(float amount, glm::vec3 axis) {
	glm::quat q = glm::angleAxis(glm::radians(amount), axis);
	this->orientation = glm::normalize(q) * this->orientation;    
}

void Camera::move(glm::vec3 direction)
{
	glm::vec3 modifiedDirection = direction * orientation;
	position += modifiedDirection * speed;
}

void Camera::selectCameraType(cameraType_e type)
{
    switch (type) {

    case perspective:
        projMatrix = glm::perspective(glm::radians(initialFoV), 16.0f / 9.0f, 0.00005f, 100.0f);
        break;

    case ortho:
        projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.05f, 100.0f);
        break;
    };
}

void Camera::step()
{

    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &screenx, &screeny);
    glfwSetCursorPos(window, 0, 0);

    zoffset = 0;
    this->xoffset = mouseSpeed * xpos;
    this->yoffset = mouseSpeed * ypos;

    checkKeys();

    rotate(xoffset, worldY);
    rotate(yoffset, worldX);
    rotate(zoffset, worldZ);


}

glm::vec3 Camera::getPosition()
{
    return position;
}

glm::mat4 Camera::getMatrix()
{
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projMatrix;
}

void Camera::setWindow(GLFWwindow* window)
{
	this->window = window;
}
