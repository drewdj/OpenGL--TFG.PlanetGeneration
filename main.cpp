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







bool renderfps(double framerate, double& deltaTime,GLFWwindow* window)
{
	static double lastTime = 0;
	static double currentTime = 0;
	int counter = 0;
	
	

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	counter++;
	if (deltaTime >= 1.0 / framerate)
	{
		std::string FPS = std::to_string((1.0 / deltaTime) * counter);
		std::string ms = std::to_string((deltaTime / counter) * 1000);
		std::string newTitle = "Planetas IV - " + FPS + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		lastTime = currentTime;
		return true;
	}
	return false;
}




int main(int argc, char** argv)
{
	double deltaTime;
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

	std::string planetVertexShader = "Shaders/Vertex/planetVertexShader.glsl";
	std::string planetControlShader = "Shaders/Tessellation/planetTessellationControlShader.glsl";
	std::string planetEvaluationShader = "Shaders/Tessellation/planetTessellationEvaluationShader.glsl";
	std::string planetFragmentShader = "Shaders/Fragment/planetFragmentShader.glsl";

	std::string atmosphereVertexShader = "Shaders/Vertex/atmosphereVertexShader.glsl";
	std::string atmosphereControlShader = "Shaders/Tessellation/atmosphereTessellationControlShader.glsl";
	std::string atmosphereEvaluationShader = "Shaders/Tessellation/atmosphereTessellationEvaluationShader.glsl";
	std::string atmosphereFragmentShader = "Shaders/Fragment/atmosphereFragmentShader.glsl";

	float separation = 5;
	int minSize = 0;
	int maxSize = 5;
	int count = 5;
	float atmosphereRatio = 1.15f;
	Object* icosahedron[5];
	Object* atmosphere[5];

	for (int i = 0; i < count; i++) {
		int size = minSize + ((maxSize - minSize) * i) / (count - 1);
		icosahedron[i] = new Icosahedron(planetVertexShader, planetControlShader, planetEvaluationShader, planetFragmentShader, size);
		icosahedron[i]->position.x = i * separation;
		render->setupObject(icosahedron[i]);
		scene->addObject(icosahedron[i]);
	}

	
	Object* light = new Cube("TRG/cube.trg");
	light->scale /= 5;
	light->position = glm::vec3(0.0f, 0.0f, 35.0f);
	render->setupObject(light);
	scene->addObject(light);

	Object* skybox = new Skybox("TRG/skybox.trg");
	render->setupObject(skybox);
	scene->addObject(skybox);

	for (int i = 0; i < count; i++) {
		atmosphere[i] = new Icosahedron(atmosphereVertexShader, "", "", atmosphereFragmentShader, 10);
		atmosphere[i]->atmosphereRadius = icosahedron[i]->radius * atmosphereRatio;
		atmosphere[i]->position.x = i * separation;
		render->setupObject(atmosphere[i]);
		scene->addObject(atmosphere[i]);
	}


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	while (!glfwWindowShouldClose(window))
	{

		if (renderfps(60.0f, deltaTime,window)) {


			scene->step(deltaTime);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			render->drawScene(scene);

			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
			
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Editor");
				ImGui::Checkbox("Ayuda ImGui", &show_demo_window);
				ImGui::Checkbox("Wireframe", &render->wireframe);
				ImGui::Checkbox("Iluminacion", &render->iluminacion);
				ImGui::Checkbox("Atmosfera", &render->atmosfera);

				for (int i = 0; i < count; i++) {
					if (ImGui::TreeNode(("Icosahedron " + std::to_string(i)).c_str())) {
						ImGui::SeparatorText("Ruido Planeta");
						ImGui::InputFloat(("Tiempo " + std::to_string(i)).c_str(), &icosahedron[i]->Time, 0.01f, 1.0f, "%.3f");
						ImGui::InputFloat(("Coordenada Textura " + std::to_string(i)).c_str(), &icosahedron[i]->textureCoord, 0.01f, 1.0f, "%.3f");
						ImGui::InputFloat(("Gradiente " + std::to_string(i)).c_str(), &icosahedron[i]->gradient, 0.005f, 1.0f, "%.3f");
						ImGui::SliderInt(("Noise Octaves " + std::to_string(i)).c_str(), &icosahedron[i]->noiseOctaves, 0.0f, 30.0f);

						ImGui::SeparatorText("Magnitud");
						ImGui::InputFloat(("Radio Planeta " + std::to_string(i)).c_str(), &icosahedron[i]->radius, 0.01f, 1.0f, "%.3f");
						ImGui::InputFloat(("Escala Atmosfera " + std::to_string(i)).c_str(), &atmosphereRatio, 0.01f, 1.0f, "%.3f");
						atmosphere[i]->atmosphereRadius = icosahedron[i]->radius * atmosphereRatio;
						

						ImGui::SeparatorText("Teselacion");
						ImGui::Checkbox(("Tessellacion Manual " + std::to_string(i)).c_str(), &icosahedron[i]->tessellationType);
						ImGui::SliderInt(("Tessellation " + std::to_string(i)).c_str(), &icosahedron[i]->tessellation, 1, 100);

						ImGui::SeparatorText("Atmosfera");
						ImGui::ColorEdit3(("Rayleigh " + std::to_string(i)).c_str(), (float*)&atmosphere[i]->rayleighScattering);

						ImGui::SeparatorText("Iluminacion");
						ImGui::ColorEdit3(("light color " + std::to_string(i)).c_str(), (float*)&lightColor); // Edit 3 floats representing a color
						icosahedron[i]->lightColor = lightColor;
						light->lightColor = lightColor;


						ImGui::TreePop();
					}
				}

				ImGui::Text("Camera speed: %.6f", scene->getCamera()->speed);
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

