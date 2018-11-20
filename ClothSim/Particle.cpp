#include "pch.h"
#include "Particle.h"

Particle::Particle()
{
}

Particle::Particle(vmath::vec3 curPos, float givenMass) {
	pos = curPos;
	prevPos = curPos;
	a = vmath::vec3(0, 0, 0);
	mass = givenMass;
	moveable = true;
	accumulatedNormal = vmath::vec3(0, 0, 0);
}

Particle::~Particle()
{
}

void Particle::setMass(float m) {
	mass = m;
}

void Particle::addForce(vmath::vec3 givenForce) {
	a += givenForce / mass;
}

void Particle::timeStep(float dampening, float t) {
	//Implement verlet
	if (moveable) {
		vmath::vec3 temp = pos;
		pos = pos + (pos - prevPos) * (1.0 - dampening) + a*(t*t);
		prevPos = temp;
		a = vmath::vec3(0,0,0);
	}
}

vmath::vec3& Particle::getPos() {
	return pos;
}

void Particle::offsetPos(const vmath::vec3 vec) {
	if (moveable) {
		pos += vec;
	}
}

void Particle::setMovable(bool mov) {
	moveable = mov;
}

void Particle::addToNormal(vmath::vec3 normal) {
	accumulatedNormal += vmath::normalize(normal);
}

//Not a unit vector
vmath::vec3& Particle::getNormal() {
	return accumulatedNormal;
}

void Particle::resetNormal() {
	accumulatedNormal = vmath::vec3(0, 0, 0);
}