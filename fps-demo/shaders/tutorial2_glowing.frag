#version 330 core
out vec4 FragColor;

uniform vec4 ourColor;

void main()
{
	FragColor = ourColor;
	//FragColor = vec4(1.0, 0.5, 1.0, 1.0);
}