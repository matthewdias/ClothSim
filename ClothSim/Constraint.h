#pragma once
#include "Particle.h"

class Constraint
{
public:
    Particle *p1, *p2;
	float restDistance;

    Constraint();
	Constraint(Particle*, Particle*);
	~Constraint();
    void satisfyConstraint();
    
private:

};
