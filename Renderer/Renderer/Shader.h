#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <glew.h>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Constants.h"

class Shader
{
public:
	Shader();
	~Shader();

	void CreateFromFiles(const char* fileVert, const char* fileGeom = nullptr, const char* fileFrag = nullptr);

	GLuint GetShaderId();

	GLint GetProjectionLocation();
	GLint GetModelLocation();
	GLint GetViewLocation();
	GLint GetCamPosLocation();
	GLint GetAmbientIntensityLocation();
	GLint GetAmbientColorLocation();
	GLint GetDiffuseIntensityLocation();
	GLint GetDiffuseDirectionLocation();
	GLint GetSpecIntensityLocation();
	GLint GetShininessLocation();

	GLint GetOmniLightPosLocation();
	GLint GetFarPlaneLocation();

	GLint GetUniformLocationByName(const char* str);

	void SetDirectionalLight(DirectionalLight* dirLight);
	void SetPointLights(PointLight* pointLights, GLint lightCount);
	void SetSpotLights(SpotLight* spotLights, GLint lightCount);

	void SetTexture(GLint textureUnit);
	void SetDirShadowMap(GLint textureUnit);
	void SetDirLightTransform(glm::mat4* transform);
	void SetDirLightTransform(glm::mat4 transform);

	void SetOmniLightTransforms(std::vector<glm::mat4> matrices);

	void UseShader();
	void ClearShader();


private:
	GLuint shaderID;
	GLint unifProjection, unifModel, unifView, unifCamPosition,
		unifSpecIntensity, unifShininess,
		unifTexture,
		unifDirLightTransform, unifDirShadowMap;

	GLint unifOmniLightPos, unifFarPlane, unifOmniLightTransforms[6];

	struct
	{
		GLint unifColor;
		GLint unifAmbientIntensity;
		GLint unifDiffuseIntensity;

		GLint unifDirection;
	} uniformDirectionalLight;


	GLint pointLightCount;
	GLint unifPointLightCount;

	struct
	{
		GLint unifColor;
		GLint unifAmbientIntensity;
		GLint unifDiffuseIntensity;

		GLint unifPosition;
		GLint unifConstant;
		GLint unifLinear;
		GLint unifExponent;
	} uniformPointLights[MAX_POINT_LIGHTS];


	GLint spotLightCount;
	GLint unifSpotLightCount;

	struct 
	{
		GLint unifColor;
		GLint unifAmbientIntensity;
		GLint unifDiffuseIntensity;

		GLint unifPosition;
		GLint unifConstant;
		GLint unifLinear;
		GLint unifExponent;

		GLint unifDirection;
		GLint unifEdge;
	} uniformSpotLights[MAX_SPOT_LIGHTS];


	void CompileShader(const char* codeVert, const char* codeGeom = nullptr, const char* codeFrag = nullptr);
	void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);
	void SetupUniforms();

	std::string ReadFile(const char* fileName);
};
