//fragment shader
#version 330

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;

out vec4 resultColor;

uniform bool myFlag;
uniform vec4 myColor;

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;

uniform sampler2D colorMap;
smooth in vec2 vVaryingTexCoords;

void main(void)
{
	if(myFlag)
	{
	//dot product gives us diffuse intensity
	float diff = max(0.0,dot(normalize(vVaryingNormal),normalize(vVaryingLightDir)));

	//multiply intensity by diffuse color ,force alpha to 1.0
	vec4 DiffColor = diff * diffuseColor;
	
	//specular Light
	vec4 SpecColor = vec4(0,0,0,0);
	vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),normalize(vVaryingNormal)));
	float spec = max(0.0,dot(normalize(vVaryingNormal),vReflection));
	if(diff != 0)
	{
		float fSpec = pow(spec,128.0);
		SpecColor.xyz += vec3(fSpec,fSpec,fSpec);
	}
	
	vec4 AmbiColor = ambientColor;

	//get texture color
	vec4 texColor = texture(colorMap,vVaryingTexCoords.st);

	//resultColor = AmbiColor + DiffColor + SpecColor + texColor;
	resultColor =  SpecColor + texColor;
	}
	else
		resultColor = myColor;
}