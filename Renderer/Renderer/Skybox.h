#pragma once

#include <vector>
#include <string>
#include <glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Constants.h"
#include "Shader.h"
#include "Mesh.h"

class Skybox
{
public:
	Skybox();
	Skybox(std::vector<std::string> faceFiles);
	~Skybox();

	void Validate();
	void DrawSkybox(glm::mat4 mtxView, glm::mat4 mtxProj);

private:
	Mesh* skyMesh;
	Shader* skyShader;

	GLuint texId;
	GLint unifProjLocation, unifViewLocation;
};

