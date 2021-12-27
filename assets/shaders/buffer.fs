#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

struct DirLight 
{
	sampler2D depthBuffer;
};
uniform DirLight dirLight;
uniform sampler2D bufferTex;

float near = 0.1;
float far = 1000.0;

void main()
{
	//float depthValue = texture(bufferTex,TexCoord).r;
	//float z = depthValue * 2.0 - 1.0;
	//float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	//float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]

	//FragColor = vec4(vec3(1-factor),1.0);
	//FragColor = vec4(0.0,0.0,1.0,1.0);
	//FragColor = texture(bufferTex,TexCoord);
	FragColor = vec4(vec3(texture(dirLight.depthBuffer,TexCoord).r),1.0);
}