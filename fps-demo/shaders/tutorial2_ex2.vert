#version 330 core
layout (location = 0) in vec3 aPos;

uniform float yOffset;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y + yOffset, aPos.z, 1.0);
}