#include "Shader.h"

Shader::Shader() :
	shaderID(0),
	unifProjection(0),
	unifModel(0)
{
}

Shader::~Shader()
{
}

void Shader::CreateFromFiles(const char* fileVert, const char* fileFrag)
{
	const auto vertCode = ReadFile(fileVert);
	const auto fragCode = ReadFile(fileFrag);
	CompileShader(vertCode.c_str(), fragCode.c_str());
}

void Shader::CreateFromString(const char* codeVert, const char* codeFrag)
{
	CompileShader(codeVert, codeFrag);
}

GLuint Shader::GetShaderId()
{
	return shaderID;
}

GLint Shader::GetProjectionLocation()
{
	return unifProjection;
}

GLint Shader::GetModelLocation()
{
	return unifModel;
}

GLint Shader::GetViewLocation()
{
	return unifView;
}

GLint Shader::GetAmbientIntensityLocation()
{
	return unifAmbIntesity;
}

GLint Shader::GetAmbientColorLocation()
{
	return unifAmbColor;
}

void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (0 != shaderID)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	unifModel = 0;
	unifProjection = 0;
}

void Shader::CompileShader(const char* codeVert, const char* codeFrag)
{
	shaderID = glCreateProgram();
	if (!shaderID)
		throw std::runtime_error("shader compilation failed");

	AddShader(shaderID, codeVert, GL_VERTEX_SHADER);
	AddShader(shaderID, codeFrag, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(errorLog), nullptr, errorLog);
		std::cout << "shader linker error" << std::endl << errorLog << std::endl;
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(errorLog), nullptr, errorLog);
		std::cout << "shader validation error" << std::endl << errorLog << std::endl;
		return;
	}

	unifModel = glGetUniformLocation(shaderID, "mtxModel");
	unifProjection = glGetUniformLocation(shaderID, "mtxProj");
	unifView = glGetUniformLocation(shaderID, "mtxView");
	unifAmbColor = glGetUniformLocation(shaderID, "dirLight.color");
	unifAmbIntesity = glGetUniformLocation(shaderID, "dirLight.ambientIntensity");
}

void Shader::AddShader(GLuint programId, const char* shaderCode, GLenum shaderType)
{
	auto thisShader = glCreateShader(shaderType);
	const GLchar* codePtr[] = { shaderCode };
	const GLint codeLength[] = { static_cast<GLint>(strlen(shaderCode)) };

	glShaderSource(thisShader, 1, codePtr, codeLength);
	glCompileShader(thisShader);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glGetShaderiv(thisShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(thisShader, sizeof(errorLog), nullptr, errorLog);
		std::cout << "shader compilation error: " << thisShader << "~" << shaderType << std::endl << errorLog << std::endl;
		return;
	}

	glAttachShader(programId, thisShader);
}

std::string Shader::ReadFile(const char* fileName)
{
	std::string content;
	std::ifstream fStream(fileName, std::ios::in);

	if (!fStream.is_open())
		throw std::runtime_error("could not open file: " + std::string(fileName));

	std::stringstream buffer;
	buffer << fStream.rdbuf();

	return buffer.str();
}
