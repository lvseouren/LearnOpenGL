#version 330 

//in
smooth in float diff;
//uniform
uniform sampler1D colorMap;
//out
out vec4 resultColor;

void main(void)
{
	resultColor = texture(colorMap,diff); 
}