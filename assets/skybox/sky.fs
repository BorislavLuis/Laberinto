#version 330 core

out vec4 FragColor;

uniform float time;
uniform vec3 min;
uniform vec3 max;

void main()
{
	float pi = atan(1.0)*4;

	for(int i = 0; i < 3;i++)
	{
		FragColor[i] = ((max[i]-min[i])/2)*sin((2*pi/24)*time-(pi/2))+((min[i]+max[i])/2);
	}
	FragColor[3]=1.0;

}
