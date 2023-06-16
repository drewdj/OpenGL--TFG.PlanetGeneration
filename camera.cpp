#include "camera.h"
#include "inputManager.h"
#include <iostream>
#include <time.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



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

void Camera::checkKeys(double deltaTime) {
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
        move(worldZ, deltaTime);
    }

    if (InputManager::keys['S'])
    {
        move(-worldZ, deltaTime);
    }

    if (InputManager::keys['A'])
    {
		move(worldX, deltaTime);
    }

    if (InputManager::keys['D'])
    {

		move(-worldX, deltaTime);
    }

    if (InputManager::keys[' '])
    {
		move(-worldY, deltaTime);
    }

    if (InputManager::keys[GLFW_KEY_LEFT_CONTROL])
    {
		move(worldY, deltaTime);
    }

	//if arrow up increase speed
	if (InputManager::keys[GLFW_KEY_UP])
	{
		speed = speed + 0.0001f;
	}

    //if arrow up increase speed
    if (InputManager::keys[GLFW_KEY_RIGHT])
    {
        speed = speed + 0.01f;
    }
    
	//if arrow down decrease speed

	if (InputManager::keys[GLFW_KEY_DOWN])
	{
		speed = speed - 0.0001f;
	}

    if (InputManager::keys[GLFW_KEY_LEFT])
    {
        speed = speed - 0.01f;
    }

    

}

void Camera::rotate(float amount, glm::vec3 axis) {
	glm::quat q = glm::angleAxis(glm::radians(amount), axis);
	this->orientation = glm::normalize(q) * this->orientation;    
}

void Camera::move(glm::vec3 direction, float deltaTime)
{
    glm::vec3 modifiedDirection = direction * orientation;
    position += modifiedDirection * (speed * deltaTime);
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

void Camera::step(double deltaTime)
{

    ImGuiIO& io = ImGui::GetIO();
    static ImVec2 prevMousePos = io.MousePos;
	float mouseDeltaX = io.MousePos.x - prevMousePos.x;
	float mouseDeltaY = io.MousePos.y - prevMousePos.y;
	ImVec2 mouseDelta = ImVec2(mouseDeltaX, mouseDeltaY);
    prevMousePos = io.MousePos;

    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
        if (ImGui::IsMouseDragging(1)) {
            this->xoffset = mouseDelta.x / 4;
            this->yoffset = mouseDelta.y / 4;
        }
        else
        {
            this->xoffset = 0;
            this->yoffset = 0;
        }
    }

    zoffset = 0;

    checkKeys(deltaTime);

    rotate(xoffset, worldY);
    rotate(yoffset, worldX);
    rotate(zoffset, worldZ);


}

glm::vec3 Camera::getForwardVector()
{
	return glm::vec3(0, 0, -1) * orientation;
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
