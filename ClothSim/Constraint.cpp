#include "pch.h"
#include "Constraint.h"


Constraint::Constraint()
{
}


Constraint::~Constraint()
{
}

Constraint::Constraint(Particle* P1, Particle* P2) {
    p1 = P1;
    p2 = P2;

    vmath::vec3 differenceVec = p1->getPos() - p2->getPos();
    restDistance = (float)vmath::length(differenceVec);    
}

void Constraint::satisfyConstraint() {
    vmath::vec3 differenceVec = p2->getPos() - p1->getPos();
    float currDistance = (float)vmath::length(differenceVec);
    vmath::vec3 correctionVec = differenceVec * (1 - restDistance/currDistance);
    vmath::vec3 correctionVecHalf = correctionVec * 0.5;
    p1->offsetPos(correctionVecHalf);
    p2->offsetPos(-correctionVecHalf);    
} 