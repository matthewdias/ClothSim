#pragma once
#include "vmath.h"
#include "Particle.h"
#include "Constraint.h"
#include <GL/glut.h>
#include <vector>
#include <string>

class Cloth
{
public:
	Cloth();
	~Cloth();

	Cloth(float, float, float, int, int, float, float, float, vmath::vec3, bool);
	void drawShaded();
	void timeStep();
	void addForce(vmath::vec3);
	void windForce(vmath::vec3);
	void ballCollision(const vmath::vec3, const float);
	void planeCollision(const float);
	void setMass(float mass);
	Particle* getParticle(int, int);

private:
	int numClothColumns;
	int numClothRows;
	float constraintIterations;
	float dampening;
	float timeUnit;

	//2D vector for all of our particles
	std::vector<Particle> particles;
	std::vector<Constraint> constraints;

	void makeConstraint(Particle*, Particle*);
	vmath::vec3 calcTriangleNormal(Particle*, Particle*, Particle*);
	void addWindPerTriangle(Particle*, Particle*, Particle*, const vmath::vec3);
	void drawTriangle(Particle*, Particle*, Particle*, vmath::vec2, vmath::vec2, vmath::vec2);
};

