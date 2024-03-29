#include "texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(std::string filename) {

	//Generacion texture ID
	unsigned char* data = NULL;
	glGenTextures(1, &glId);

	//Carga de fichero
	data = stbi_load(filename.c_str(), &w, &h, nullptr, 4);

	//carga de datos en GPU
	glBindTexture(GL_TEXTURE_2D, glId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//liberar datos? cuando se dejende usar
	stbi_image_free(data);

}

Texture::Texture(const std::vector<std::string>& filenames) {
	// Genera un ID de textura para cada archivo en el vector
	glIds.resize(filenames.size());

	glGenTextures(filenames.size(), glIds.data());

	for (size_t i = 0; i < filenames.size(); ++i) {
		// Carga la imagen del archivo
		unsigned char* data = stbi_load(filenames[i].c_str(), &w, &h, nullptr, 4);

		// Carga los datos en la GPU
		glBindTexture(GL_TEXTURE_2D, glIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Libera los datos de la imagen
		stbi_image_free(data);
	}
}

	Texture::Texture(int textType, std::string folder) {
		
		this->textType = textType;

		if (folder != "")
		{
			// All the faces of the cubemap (make sure they are in this exact order)

			std::string carpeta = "data/" + folder + "/";

			std::string facesCubemap[6] =
			{
				carpeta + "right.jpg",
				carpeta + "left.jpg",
				carpeta + "top.jpg",
				carpeta + "bot.jpg",
				carpeta + "front.jpg",
				carpeta + "back.jpg"
			};

			// Creates the cubemap texture object
			unsigned int cubemapTexture = -1;
			glGenTextures(1, &cubemapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			// These are very important to prevent seams
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			// This might help with seams on some systems
			//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			// Cycles through all the textures and attaches them to the cubemap object
			for (unsigned int i = 0; i < 6; i++)
			{
				int width, height, nrChannels;
				unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					stbi_set_flip_vertically_on_load(false);
					glTexImage2D
					(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0,
						GL_RGB,
						width,
						height,
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						data
					);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
					stbi_image_free(data);
				}
			}

			this->glId = cubemapTexture;
		}
		
	}

	Texture::Texture() {
	}

	Texture::Texture(int textType) {
		this->textType = textType;
	}
	
	void Texture::bindMultiple(int startingTextureUnitIdx) {
		for (size_t i = 0; i < glIds.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + startingTextureUnitIdx + i);
			glBindTexture(GL_TEXTURE_2D, glIds[i]);
		}
	}

	
void Texture::bind(int textureunitIdx) {
	if (this->textType == PLANET)
	{
		glActiveTexture(GL_TEXTURE0 + textureunitIdx);
		glBindTexture(GL_TEXTURE_3D, glId);
	}
	else {
		glActiveTexture(GL_TEXTURE0 + textureunitIdx);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glId);
	}

}
