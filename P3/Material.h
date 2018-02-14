#ifndef MATERIAL_H
#define MATERIAL_H

class Material {
public:
	float ka; //ambient reflection coefficient
	float kd; //diffuse reflection coefficient
	float ks; //specular reflection coefficient
	int n;
};

#endif