
#version 330 core
layout (location = 0) in vec3 vertexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float vertexHeight;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	gl_PointSize = 10.0;
	vertexHeight = vertexPos.y;
}