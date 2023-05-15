#include "render.h"
#include <time.h>
#include <cstdlib>


Render::Render(){
	glEnable(GL_DEPTH_TEST);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	//TODO: sin esto la atmosfera marca triangulos
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	
}


void Render::setupObject(Object* obj)
{

	bufferObject_t bo;
	glGenVertexArrays(1, &bo.abo);
	glBindVertexArray(bo.abo);
	
	glGenBuffers(1,&bo.vbo);
	glGenBuffers(1,&bo.ibo);

	glBindBuffer(GL_ARRAY_BUFFER,bo.vbo);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_t)*obj->mesh->vertexList->size(),
					obj->mesh->vertexList->data(),GL_STATIC_DRAW);
					
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bo.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*obj->mesh->faceList->size(),
					obj->mesh->faceList->data(),GL_STATIC_DRAW);

	boList[obj->id]=bo;	
}


void Render::drawMesh(Mesh* mesh,glm::mat4 model){


	int numFaces=(int)mesh->faceList->size()/3;
	for(int i=0;i<numFaces;i++)
	{
	glBegin(GL_TRIANGLES);
		glm::vec4 newPos;	
		for(int idV=i*3; idV<(i*3+3); idV++)
		{   
			vertex_t v=mesh->vertexList->data()[mesh->faceList->data()[idV]];
			newPos=proj*view*model*v.posicion;
			glColor3f(v.color.r,v.color.g,v.color.b);
			glVertex3f(newPos.x,newPos.y,newPos.z);
		}
	
	glEnd();
	}
}


void Render::drawObject(Object* obj){
	obj->computeMatrix();
	drawMesh(obj->mesh,obj->getMatrix());	
}

void Render::drawObjectGL4(Object* obj){

	if (obj->mesh->tex->textType == SKYBOX) {
		glDepthFunc(GL_LEQUAL);
	}
	
	if (obj->mesh->tex->textType == ATMOSPHERE) {
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		
	}

	obj->computeMatrix();

	glm::vec4 lightColor = obj->lightColor;
	
	bufferObject_t bo=boList[obj->id];
	
	glBindVertexArray(bo.abo);
	glBindBuffer(GL_ARRAY_BUFFER, bo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.ibo);


	glUseProgram(obj->shader->programID);
	
	unsigned int vpos=0;
	glEnableVertexAttribArray(vpos);
	glVertexAttribPointer(vpos,4,GL_FLOAT,GL_FALSE,sizeof(vertex_t),(void*)offsetof(vertex_t,posicion));

	unsigned int vcolor=1;
	glEnableVertexAttribArray(vcolor);
	glVertexAttribPointer(vcolor,4,GL_FLOAT,GL_FALSE,sizeof(vertex_t),(void*)offsetof(vertex_t,color));

	unsigned int vnorm=2;
	glEnableVertexAttribArray(vnorm);
	glVertexAttribPointer(vnorm,4,GL_FLOAT,GL_FALSE,sizeof(vertex_t),(void*)offsetof(vertex_t,normal));
	
	glm::vec4 lightPos(0.0f,0.0f,5.0f,1.0f);

	int textureUnit = 0;	
	if (obj->mesh->tex->textType == PLANET)
	{
		obj->mesh->tex->bindMultiple(textureUnit);

	}
	else {
		
		obj->mesh->tex->bind(textureUnit);
	}

	glm::mat4 testView = view;
	
	if (obj->mesh->tex->textType == SKYBOX) {		
		//downgrade to mat3 and scale it back last row = 0 so no effects on traslation
		testView = glm::mat4(glm::mat3(view));
	}

	// Asume que 'shader' es un objeto que representa tu programa de shader
	for (size_t i = 0; i < 3; ++i) {
		std::string uniformName = "u_Textures[" + std::to_string(i) + "]";
		GLint location = glGetUniformLocation(obj->mesh->shader->programID, uniformName.c_str());
		glUniform1i(location, textureUnit + i); // Enlaza la unidad de textura al sampler en el shader
	}

	
	glUniformMatrix4fv(glGetUniformLocation(obj->shader->programID, "MVP"), 1, GL_FALSE, &(proj * testView * obj->getMatrix())[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(obj->shader->programID, "M"), 1, GL_FALSE, &(obj->getMatrix())[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(obj->shader->programID, "V"), 1, GL_FALSE, &(view)[0][0]);
	
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "lightPos"), 1, &lightPos[0]);
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "lightColor"), 1, &lightColor[0]);

	//ruido
	glUniform1f(glGetUniformLocation(obj->shader->programID, "time"), (float)clock() / CLOCKS_PER_SEC);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "manualTime"), obj->testTime);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "textureCoord"), obj->textureCoord);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "gradient"), obj->gradient);

	glUniform4fv(2,1,&lightPos[0]);

	//skybox
	glUniform1i(glGetUniformLocation(obj->shader->programID, "textureUnit"), textureUnit);
	
	//light
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "lightPos"), 1, &lightPos[0]);
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "lightColor"), 1, &lightColor[0]);

	glUniform4fv(glGetUniformLocation(obj->shader->programID, "waterColor"),1,&obj->waterColor[0]);
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "landColor"), 1, &obj->landColor[0]);
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "mountainColor"), 1, &obj->mountainColor[0]);
	glUniform1i(glGetUniformLocation(obj->shader->programID, "tessellation"), obj->tessellation);

	glUniform1f(glGetUniformLocation(obj->shader->programID, "waterLevel"), obj->waterLevel);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "landLevel"), obj->landLevel);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "mountainLevel"), obj->mountainLevel);

	//planeta
	glUniform1f(glGetUniformLocation(obj->shader->programID, "planetRadius"), obj->planetRadius);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "atmosphereRadius"), obj->atmosphereRadius);
	glUniform3fv(glGetUniformLocation(obj->shader->programID, "rayleighScattering"), 1, &obj->rayleighScattering[0]);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "mieScattering"), obj->mieScattering);
	glUniform2fv(glGetUniformLocation(obj->shader->programID, "hesightScale"), 1, &obj->hesightScale[0]);
	glUniform1f(glGetUniformLocation(obj->shader->programID, "refraction"), obj->refraction);

	
	//camara
	glUniform4fv(glGetUniformLocation(obj->shader->programID, "camPos"), 1, &cam->getPosition()[0]);

	//Pintar lineas
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);


	if (obj->mesh->tex->textType == PLANET || obj->mesh->tex->textType == 7) {
		glDrawElements(GL_PATCHES, obj->mesh->faceList->size(), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, obj->mesh->faceList->size(), GL_UNSIGNED_INT, nullptr);
	}

	if (obj->mesh->tex->textType == SKYBOX) {
		glDepthFunc(GL_LESS);
	}
	
	if (obj->mesh->tex->textType == ATMOSPHERE) {
		glDisable(GL_BLEND);
	}
	
}


void Render::drawScene(Scene* scene)
{

	Camera* cam=scene->getCamera();
	std::map<int,Object*> *addedObjList=scene->addedObjList;
	
	for(auto it=addedObjList->begin();
            it!=addedObjList->end();
            it++)
    {

        setupObject(it->second);
    }
    
	cam->computeMatrix();
	view=cam->getMatrix();
	proj=cam->getProjectionMatrix();
	std::map<int,Object*>* objList=scene->getObjList();	
	
	for(auto it=objList->begin();
		it!=objList->end();
		it++)
	{
		drawObjectGL4(it->second);
	}

}

void Render::setCamera(Camera* cam) {

	this->cam = cam;
}









