#include<stdio.h>

struct Vec {
	float x,y,z;
	float u,v;
};

void init_z(struct Vec *v, float z);


int main() {
	struct Vec v;
	init_z(&v, 1.1f);
	printf("z value is: %g\n", v.z);
	return 0;
}

void init_z(struct Vec *v, float z) {
	*v = (struct Vec) {
		.x = 0.0f,
		.y = 0.0f,
		.z = z,
		.u = 0.0f,
		.v = 1.0f
	};
}
