#include "Shader.h"

Shader::Shader() :
	shaderID(0),
	unifProjection(0),
	unifModel(0),
	pointLightCount(0),
	spotLightCount(0)
{
}

Shader::~Shader()
{
}

void Shader::CreateFromFiles(const char* fileVert, const char* fileGeom, const char* fileFrag)
{
	std::string codeVert;
	std::string codeGeom;
	std::string codeFrag;

	codeVert = ReadFile(fileVert);
	if (fileGeom)
		codeGeom = ReadFile(fileGeom);
	if (fileFrag)
		codeFrag = ReadFile(fileFrag);

	CompileShader
	(
		codeVert.empty() ? nullptr : codeVert.c_str(),
		codeGeom.empty() ? nullptr : codeGeom.c_str(),
		codeFrag.empty() ? nullptr : codeFrag.c_str()
	);
	SetupUniforms();
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

GLint Shader::GetCamPosLocation()
{
	return unifCamPosition;
}

GLint Shader::GetAmbientIntensityLocation()
{
	return uniformDirectionalLight.unifAmbientIntensity;
}

GLint Shader::GetAmbientColorLocation()
{
	return uniformDirectionalLight.unifColor;
}

GLint Shader::GetDiffuseIntensityLocation()
{
	return uniformDirectionalLight.unifDiffuseIntensity;
}

GLint Shader::GetDiffuseDirectionLocation()
{
	return uniformDirectionalLight.unifDirection;
}

GLint Shader::GetSpecIntensityLocation()
{
	return unifSpecIntensity;
}

GLint Shader::GetShininessLocation()
{
	return unifShininess;
}

GLint Shader::GetOmniLightPosLocation()
{
	return unifOmniLightPos;
}

GLint Shader::GetFarPlaneLocation()
{
	return unifFarPlane;
}

GLint Shader::GetUniformLocationByName(const char* str)
{
	return glGetUniformLocation(shaderID, str);
}

void Shader::SetDirectionalLight(DirectionalLight* dirLight)
{
	dirLight->UseLight(
		uniformDirectionalLight.unifColor,
		uniformDirectionalLight.unifAmbientIntensity,
		uniformDirectionalLight.unifDiffuseIntensity,
		uniformDirectionalLight.unifDirection);
}

void Shader::SetPointLights(PointLight* pointLights, GLint lightCount)
{
	if (lightCount > MAX_POINT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

	glUniform1i(unifPointLightCount, lightCount);

	for (GLint i = 0; i < lightCount; ++i)
	{
		pointLights[i].UseLight(
			uniformPointLights[i].unifColor,
			uniformPointLights[i].unifAmbientIntensity,
			uniformPointLights[i].unifDiffuseIntensity,
			uniformPointLights[i].unifPosition,
			uniformPointLights[i].unifConstant,
			uniformPointLights[i].unifLinear,
			uniformPointLights[i].unifExponent
		);
	}
}

void Shader::SetSpotLights(SpotLight* spotLights, GLint lightCount)
{
	if (lightCount > MAX_SPOT_LIGHTS)
		lightCount = MAX_SPOT_LIGHTS;

	glUniform1i(unifSpotLightCount, lightCount);

	for (GLint i = 0; i < lightCount; ++i)
	{
		spotLights[i].UseLight(
			uniformSpotLights[i].unifColor,
			uniformSpotLights[i].unifAmbientIntensity,
			uniformSpotLights[i].unifDiffuseIntensity,
			uniformSpotLights[i].unifPosition,
			uniformSpotLights[i].unifDirection,
			uniformSpotLights[i].unifConstant,
			uniformSpotLights[i].unifLinear,
			uniformSpotLights[i].unifExponent,
			uniformSpotLights[i].unifEdge
		);
	}
}

void Shader::SetTexture(GLint textureUnit)
{
	glUniform1i(unifTexture, textureUnit);
}

void Shader::SetDirShadowMap(GLint textureUnit)
{
	glUniform1i(unifDirShadowMap, textureUnit);
}

void Shader::SetDirLightTransform(glm::mat4* transform)
{
	glUniformMatrix4fv(unifDirLightTransform, 1, GL_FALSE, value_ptr(*transform));
}

void Shader::SetDirLightTransform(glm::mat4 transform)
{
	glUniformMatrix4fv(unifDirLightTransform, 1, GL_FALSE, value_ptr(transform));
}

void Shader::SetOmniLightTransforms(std::vector<glm::mat4> matrices)
{
	for (size_t i = 0; i < 6; ++i)
		glUniformMatrix4fv(unifOmniLightTransforms[i], 1, GL_FALSE, value_ptr(matrices[i]));
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

void Shader::CompileShader(const char* codeVert, const char* codeGeom, const char* codeFrag)
{
	shaderID = glCreateProgram();
	if (!shaderID)
		throw std::runtime_error("shader compilation failed");

	AddShader(shaderID, codeVert, GL_VERTEX_SHADER);
	if (nullptr != codeGeom)
		AddShader(shaderID, codeGeom, GL_GEOMETRY_SHADER);
	if (nullptr != codeFrag)
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

void Shader::SetupUniforms()
{
	unifModel = glGetUniformLocation(shaderID, "mtxModel");
	unifProjection = glGetUniformLocation(shaderID, "mtxProj");
	unifView = glGetUniformLocation(shaderID, "mtxView");
	unifCamPosition = glGetUniformLocation(shaderID, "eyePos");

	uniformDirectionalLight.unifColor = glGetUniformLocation(shaderID, "dirLight.base.color");
	uniformDirectionalLight.unifAmbientIntensity = glGetUniformLocation(shaderID, "dirLight.base.ambientIntensity");
	uniformDirectionalLight.unifDirection = glGetUniformLocation(shaderID, "dirLight.direction");
	uniformDirectionalLight.unifDiffuseIntensity = glGetUniformLocation(shaderID, "dirLight.base.diffuseIntensity");

	unifSpecIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
	unifShininess = glGetUniformLocation(shaderID, "material.shininess");

	unifPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

	for (GLint i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		char locBuff[100] = { '\0' };
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
		uniformPointLights[i].unifColor = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLights[i].unifAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLights[i].unifDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
		uniformPointLights[i].unifPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
		uniformPointLights[i].unifConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
		uniformPointLights[i].unifLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
		uniformPointLights[i].unifExponent = glGetUniformLocation(shaderID, locBuff);
	}


	unifSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

	for (GLint i = 0; i < MAX_SPOT_LIGHTS; ++i)
	{
		char locBuff[100] = { '\0' };
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.color", i);
		uniformSpotLights[i].unifColor = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLights[i].unifAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLights[i].unifDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
		uniformSpotLights[i].unifPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
		uniformSpotLights[i].unifConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
		uniformSpotLights[i].unifLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
		uniformSpotLights[i].unifExponent = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLights[i].unifDirection = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLights[i].unifEdge = glGetUniformLocation(shaderID, locBuff);
	}

	unifTexture = glGetUniformLocation(shaderID, "texture0");
	unifDirLightTransform = glGetUniformLocation(shaderID, "mtxDirLightTransform");
	unifDirShadowMap = glGetUniformLocation(shaderID, "dirShadowMap");

	unifOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
	unifFarPlane = glGetUniformLocation(shaderID, "farPlane");

	for (GLint i = 0; i < 6; ++i)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "lightTransforms[%d]", i);
		unifOmniLightTransforms[i] = glGetUniformLocation(shaderID, locBuff);
	}
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
