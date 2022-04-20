#version 330

in vec4 vCol;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragWorldPos;
in vec4 DirLightSpacePos;

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

uniform sampler2D texture0;
uniform sampler2D dirShadowMap;

uniform Material material;

uniform vec3 eyePos;

float CalcDirShadowFactor(DirectionalLight light)
{
	vec3 projCoords = DirLightSpacePos.xyz / DirLightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	
	//bias
	vec3 bias_nrm = normalize(Normal);
	vec3 bias_lightdir = normalize(-light.direction);
	float bias = max(0.05 * (1 - dot(bias_nrm, bias_lightdir)), 0.005);
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.f : 0.f;
		}		
	}
	shadow /= 9.0;
	
	if (projCoords.z > 1.0)
		shadow = 0.0;
	
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambColor = vec4(light.color, 1.f) * light.ambientIntensity;
	
	float diffuseFactor = dot(normalize(Normal), normalize(direction)); 
	diffuseFactor = max(diffuseFactor, 0.f);
	vec4 diffuseColor = vec4(light.color, 1.f) * light.diffuseIntensity * diffuseFactor;
	
	vec4 specColor = vec4(0.f);
	if (diffuseFactor > 0.f)
	{
		vec3 fragToEyeDir = normalize(eyePos - FragWorldPos);
		vec3 reflectedLightDir = normalize(reflect(-direction, normalize(Normal)));
		float specFactor = dot(fragToEyeDir, reflectedLightDir);
		if (specFactor > 0.f)
		{
			specFactor = pow(specFactor, material.shininess);
			specColor = vec4(light.color * material.specularIntensity * specFactor, 1.f);
		}
	}
	
	return ambColor + (1.0 - shadowFactor) * (diffuseColor + specColor);
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirShadowFactor(dirLight);
	return CalcLightByDirection(dirLight.base, -dirLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight)
{
	vec3 dir = pLight.position - FragWorldPos;
	float dist = length(dir);
	dir = normalize(dir);
		
	vec4 col = CalcLightByDirection(pLight.base, dir, 0.f);
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
	
	outputCol = texture(texture0, TexCoord0) * finalLight;
}