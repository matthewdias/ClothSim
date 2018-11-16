#pragma once
#include "vmath.h"

class Particle
{
private:
	bool moveable;
	float mass;
	vmath::vec3 pos;
	vmath::vec3 prevPos;
	vmath::vec3 a;
	vmath::vec3 accumulatedNormal;

public:
	Particle(vmath::vec3, float);
	Particle();
	~Particle();

	void addForces(vmath::vec3);

	void timeStep(float, float);
	vmath::vec3& getPos();
	void offsetPos(const vmath::vec3);
	void setMoveable(bool);
	void addToNormal(vmath::vec3);
	vmath::vec3& getNormal();
	void resetNormal();
};

