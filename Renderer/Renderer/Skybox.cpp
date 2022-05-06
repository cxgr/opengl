#include "Skybox.h"

Skybox::Skybox()
{
}

Skybox::Skybox(std::vector<std::string> faceFiles)
{
	skyShader = new Shader();
	skyShader->CreateFromFiles("Shaders/Skybox.vert", nullptr, "Shaders/Skybox.frag");

	unifProjLocation = skyShader->GetProjectionLocation();
	unifViewLocation = skyShader->GetViewLocation();

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	GLsizei wid, hei, bitDepth;

	for (size_t i = 0; i < 6; ++i)
	{
		unsigned char* texData = stbi_load(faceFiles[i].c_str(), &wid, &hei, &bitDepth, 0);
		if (!texData)
		{
			std::cout << "failed to load texture: " << faceFiles[i] << std::endl;
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, wid, hei, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::vector<unsigned> skyboxIndices
	{
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	std::vector<float> skyboxVertices
	{
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices.data(), skyboxVertices.size(), skyboxIndices.data(), skyboxIndices.size());
}

Skybox::~Skybox()
{
}

void Skybox::Validate()
{
	skyShader->Validate();
}

void Skybox::DrawSkybox(glm::mat4 mtxView, glm::mat4 mtxProj)
{
	mtxView = glm::mat4(glm::mat3(mtxView));

	glDepthMask(GL_FALSE);

	skyShader->UseShader();
	glUniformMatrix4fv(unifViewLocation, 1, GL_FALSE, glm::value_ptr(mtxView));
	glUniformMatrix4fv(unifProjLocation, 1, GL_FALSE, glm::value_ptr(mtxProj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	skyMesh->RenderMesh();

	glDepthMask(GL_TRUE);
}
