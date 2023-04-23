#pragma once

#include "common.h"
#include "mesh.h"
#include "collider.h"
#include "shader.h"

class Object{

	static int idCounter;
public:
	int id=0;
	int typeObject;
	bool markedDelete=false;
	
	float testTime = 0.0f;
	float textureCoord = 0.0f;
	float gradient = 0.0f;

	float radius = 1.0f;

	glm::vec4 waterColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 landColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 mountainColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	float waterLevel = 0.0f;
	float landLevel = 0.0f;
	float mountainLevel = 0.0f;
	
	int tessellation = 64;

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	
	glm::mat4 modelMatrix;
	glm::vec3 position=glm::vec3(0,0,0);
	glm::vec3 rotation=glm::vec3(0,0,0);
	glm::vec3 scale=glm::vec3(1.0f,1.0f,1.0f);

	Mesh* mesh;
	GLShader* shader;
	
	Collider* collider;
		
	Object();
	Object(std::string vertexShader, std::string tessellationControlShader, std::string tessellationEvaluationShader, std::string fragmentShader, int vertex);
	Object(std::string fileName);
	glm::mat4 getMatrix();
	void computeMatrix();
	void updateCollider();
	virtual void step();
	
	
};
