#version 330

//in
smooth in vec2 vVaryingTexCoords;

//out
out vec4 resultColor;


//uniform 
uniform sampler2D colorMap;

void main()
{
	resultColor = texture(colorMap,vVaryingTexCoords.st);	
}