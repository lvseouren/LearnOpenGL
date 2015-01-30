#version 330

smooth in vec3 vVaryingTexCoord;

uniform samplerCube cubeMap;

out vec4 resultColor;

void main(void)
{
	resultColor = texture(cubeMap,vVaryingTexCoord.stp);
}