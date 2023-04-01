#pragma once
#include "object.h"

class Skybox : public Object {
public:
	Skybox(std::string fileName) :Object(fileName) { typeObject = SKYBOX_OBJ; };
	virtual void step() override;
};

