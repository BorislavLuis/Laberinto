layout(std40) uniform Test{
	int i;

	struct t2
	{
		int d[5];
		float f;
		vec3 v[2];
	};
	struct t3
	{
		mat3 m;
		vec3 v;
	}a[2];
};