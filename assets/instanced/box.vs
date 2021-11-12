#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffeset;
layout (location = 2) in vec3 aSize;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 pos = vec3(aPos.x*aSize.x,aPos.y*aSize.y,aPos.z*aSize.z);
	gl_Position = projection * view * vec4(pos + aOffest,1.0);
}