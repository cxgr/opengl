#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <glew.h>
#include <sstream>

class Shader
{
public:
	Shader();
	~Shader();

	void CreateFromFiles(const char* fileVert, const char* fileFrag);
	void CreateFromString(const char* codeVert, const char* codeFrag);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();

	void UseShader();
	void ClearShader();


private:
	GLuint shaderID;
	GLuint uniformProjection, uniformModel, uniformView;

	void CompileShader(const char* codeVert, const char* codeFrag);
	void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);

	std::string ReadFile(const char* fileName);
};
