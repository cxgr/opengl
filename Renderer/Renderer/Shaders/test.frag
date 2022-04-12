#version 330

in vec4 vCol;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragWorldPos;

out vec4 outputCol;

const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform DirectionalLight dirLight;
uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D sampler0;
uniform Material material;

uniform vec3 eyePos;

vec4 CalcLightByDirection(Light light, vec3 direction)
{
	vec4 ambColor = vec4(light.color, 1.f) * light.ambientIntensity;
	
	float diffuseFactor = dot(normalize(Normal), normalize(direction)); 
	diffuseFactor = max(diffuseFactor, 0.f);
	vec4 diffuseColor = vec4(light.color, 1.f) * light.diffuseIntensity * diffuseFactor;
	
	vec4 specColor = vec4(0.f);
	if (diffuseFactor > 0.f)
	{
		vec3 fragToEyeDir = normalize(eyePos - FragWorldPos);
		vec3 reflectedLightDir = normalize(reflect(direction, normalize(Normal)));
		float specFactor = dot(fragToEyeDir, reflectedLightDir);
		if (specFactor > 0.f)
		{
			specFactor = pow(specFactor, material.shininess);
			specColor = vec4(light.color * material.specularIntensity * specFactor, 1.f);
		}
	}
	
	return ambColor + diffuseColor + specColor;
}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(dirLight.base, dirLight.direction);
}

vec4 CalcPointLight(PointLight pLight)
{
	vec3 dir = pLight.position - FragWorldPos;
	float dist = length(dir);
	dir = normalize(dir);
		
	vec4 col = CalcLightByDirection(pLight.base, dir);
	float attenuation = pLight.exponent * dist * dist + 
						pLight.linear * dist +
						pLight.constant;
	return col / attenuation;
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0.f);
	
	for (int i = 0; i < pointLightCount; ++i)
	{
		totalColor += CalcPointLight(pointLights[i]);
	}
	
	return totalColor;
}

vec4 CalcSpotLight(SpotLight sLight)
{
	vec3 rayDir = normalize(FragWorldPos - sLight.base.position);
	float slFactor = dot(rayDir, sLight.direction);
	
	if (slFactor > sLight.edge)
	{
		vec4 spotCol = CalcPointLight(sLight.base);
		float remapCoeff = 1.f - (1.f - slFactor) * (1.f / (1.f - sLight.edge));
		return spotCol * remapCoeff;
	}
	else
	{
		return vec4(0.f);
	}
}

vec4 CalcSpotLights()
{
	vec4 totalColor = vec4(0.f);
	
	for (int i = 0; i < spotLightCount; ++i)
	{
		totalColor += CalcSpotLight(spotLights[i]);
	}
	
	return totalColor;
}

void main()
{
	vec4 finalLight = CalcDirectionalLight();
	finalLight += CalcPointLights();
	finalLight += CalcSpotLights();
	
	outputCol = texture(sampler0, TexCoord0) * finalLight;
}