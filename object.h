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
	
	float Time = 0.0f;
	float textureCoord = 0.0f;
	float gradient = 0.0f;

	float planetRadius = 1.0f;
	float atmosphereRadius = 1.0f;
	glm::vec3 rayleighScattering = glm::vec3(5.5e-6, 13.0e-6, 22.4e-6);
	float mieScattering = 21e-6;
	glm::vec2 hesightScale = glm::vec2(8.0, 1.2);
	float refraction = 0.76f;
	
	float colorTime = 1.0f;
	float colorTextureCoord = 0.0f;
	float colorGradient = 0.0f;

	int noiseOctaves = 8.0f;
	float noiseAmplitude = 0.5f;
	float noiseFrequency = 2.0f;
	float noiseN = 0.5f;
	
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
