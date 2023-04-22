#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream> 
#include "common.h"
#include <vector>
#include "mesh.h"
#include "render.h"
#include "object.h"
#include "scene.h"
#include "inputManager.h"
#include "system.h"

#include "cube.h"
#include "skybox.h"
#include "icosahedron.h"







bool renderfps(double framerate, GLFWwindow* window)
{
	static double lastTime = 0;
	static double currentTime = 0;
	double timeDiff;
	int counter = 0;
	
	

	currentTime = glfwGetTime();
	timeDiff = currentTime - lastTime;
	counter++;
	if (timeDiff >= 1.0 / framerate)
	{
		std::string FPS = std::to_string((1.0 / timeDiff) * counter);
		std::string ms = std::to_string((timeDiff / counter) * 1000);
		std::string newTitle = "Planetas IV - " + FPS + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		lastTime = currentTime;
		return true;
	}
	return false;
}




int main(int argc, char** argv)
{

	int glfwState = glfwInit();
	if (!glfwState)
		std::cout << "ERROR iniciando glfw\n";


	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Prueba 1 GLFW", nullptr, nullptr);
	//arregla problema de posicion inicial con la camara
	glfwSetCursorPos(window, 0, 0);
	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//TODO
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	


	int glewState = glewInit();

	if (glewState != GLEW_OK)
		std::cout << "ERROR iniciando glew\n";

	InputManager::init(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	
	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	glm::vec4 test = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);


	Render* render = new Render();
	Scene* scene = new Scene();
	System::scene = scene;
	scene->setCamera(new Camera(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0.25), perspective),window);
	render->setCamera(scene->getCamera());

	Object* icosahedron = new Icosahedron(0);
	icosahedron->position.z -= 2;
	render->setupObject(icosahedron);
	scene->addObject(icosahedron);	

	Object* light = new Cube("TRG/cube.trg");
	light->scale /= 5;
	light->position = glm::vec3(0.0f, 0.0f, 5.0f);
	render->setupObject(light);
	scene->addObject(light);

	Object* skybox = new Skybox("TRG/skybox.trg");
	render->setupObject(skybox);
	scene->addObject(skybox);

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	while (!glfwWindowShouldClose(window))
	{

		if (renderfps(60.0f, window)) {



			
			scene->step(0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			render->drawScene(scene);

			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
			
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it
				ImGui::Checkbox("Demo Window", &show_demo_window);
				ImGui::SliderFloat("time", &icosahedron->testTime, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("textureCoord", &icosahedron->textureCoord, 0.0f, 10.0f);
				ImGui::SliderFloat("g", &icosahedron->gradient, 0.0f, 1.0f);
				ImGui::SliderInt("Tessellation", &icosahedron->tessellation, 1, 64);
				ImGui::Separator();

				ImGui::ColorEdit3("light color", (float*)&lightColor); // Edit 3 floats representing a color
				icosahedron->lightColor = lightColor;
				light->lightColor = lightColor;
				ImGui::Separator();

				//select color ImGui
				ImGui::ColorEdit3("water color", (float*)&icosahedron->waterColor); // Edit 3 floats representing a color
				ImGui::ColorEdit3("land color", (float*)&icosahedron->landColor); // Edit 3 floats representing a color
				ImGui::ColorEdit3("mountain color", (float*)&icosahedron->mountainColor); // Edit 3 floats representing a color
				ImGui::Separator();

				ImGui::SliderFloat("water level", &icosahedron->waterLevel, 0.0f, 1.0f);
				ImGui::SliderFloat("land level", &icosahedron->landLevel, 0.0f, 1.0f);
				ImGui::SliderFloat("mountain level", &icosahedron->mountainLevel, 0.0f, 1.0f);
				ImGui::Separator();

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			
			
			glfwSwapBuffers(window);
			glfwPollEvents();

			//close window
			if (InputManager::keys['C'])
			{
				glfwSetWindowShouldClose(window, true);
			}
		}

	}

	//delete icosahedron;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	return 0;

}

