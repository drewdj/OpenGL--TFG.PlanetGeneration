#pragma once

#include "common.h"


typedef struct vertex_t{

	glm::vec4 posicion;
	glm::vec4 color;
	std::vector<glm::vec4>* faceNormals;
	glm::vec4 normal;
	int positionInList;
}vertex_t;


