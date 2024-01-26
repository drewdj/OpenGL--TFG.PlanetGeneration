#pragma once
#include "object.h"


class Icosahedron: public Object
{
public:

	Icosahedron(std::string vertexShader, std::string tessellationControlShader, std::string tessellationEvaluationShader, std::string fragmentShader, int vertex) :Object(vertexShader, tessellationControlShader, tessellationEvaluationShader, fragmentShader, vertex) { typeObject = CUBE_OBJ; };
	virtual void step() override;
	
};

