layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aOffset;
layout (location = 5) in vec3 aSize;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;

	TangentLights tanLights;
} vs_out;

uniform mat4 model;
uniform mat3 normalModel;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main()
{
	vec3 pos = aPos * aSize + aOffset;

	vs_out.FragPos = vec3(model*vec4(pos,1.0));
	vs_out.TexCoord = aTexCoord;

	vs_out.tanLights.Normal = normalize(normalModel * aNormal);

	vec3 T = normalize(normalModel * aTangent);
	vec3 N = vs_out.tanLights.Normal;
	T = normalize(T - dot(T,N)*N);
	vec3 B = cross(N,T);
	mat3 TBNinv = transpose(mat3(T,B,N));
	
	vs_out.tanLights.FragPos = TBNinv*vs_out.FragPos;
	vs_out.tanLights.ViewPos = TBNinv*viewPos;

	vs_out.tanLights.dirLightDirection = TBNinv * dirLight.direction;

	for(int i = 0;i < noPointLights;i++)
	{
		vs_out.tanLights.pointLightPositions[i] = TBNinv * pointLight[i].position;
	}
	for(int i = 0;i < noSpotLights;i++)
	{
		vs_out.tanLights.spotLightPositions[i] = TBNinv * spotLight[i].position;
		vs_out.tanLights.spotLightDirections[i] = TBNinv * spotLight[i].direction;
	}

	gl_Position = projection * view * vec4(vs_out.FragPos,1.0);
}