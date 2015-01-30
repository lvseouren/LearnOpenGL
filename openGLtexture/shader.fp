#version 330

//in
smooth in vec2 vVaryingTexCoord;
//uniform
uniform sampler2DRect colorMap;
//out
out vec4 resultColor;

void main(void)
{
	resultColor = texture(colorMap,vVaryingTexCoord);
}