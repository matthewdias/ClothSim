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
    vmath::vec3 normalDifferenceVec = vmath::normalize(differenceVec);
    float currDistance = (float)vmath::length(differenceVec);
    vmath::vec3 velocity1 = p1->pos - p1->prevPos;
    vmath::vec3 velocity2 = p2->pos - p2->prevPos;
    float v1 = vmath::dot(normalDifferenceVec, velocity1);
    float v2 = vmath::dot(normalDifferenceVec, velocity2);

    // Ks is the stiffness of the spring, when k gets bigger, the spring wants keep it's length
    float springForce = -0.1f * (restDistance - currDistance);
    float springDamp = -0.5f * (v1 - v2);
    float totalSpringForce = springForce + springDamp;
    vmath::vec3 f1 = totalSpringForce * normalDifferenceVec;

    // // Ks is the stiffness of the spring, when k gets bigger, the spring wants keep it's length
    // vmath::vec3 springForce = -0.04f * (restDistance - currDistance) * (differenceVec / currDistance);
    // vmath::vec3 springDamp = -0.01f * ((p1->pos - p1->prevPos) - (p2->pos - p2->prevPos));
    // vmath::vec3 totalSpringForce = springForce + springDamp;
    
    p1->addForce(-f1);
    p2->addForce(f1);
/*
    vmath::vec3 correctionVec = differenceVec * (1 - restDistance/currDistance);
    vmath::vec3 correctionVecHalf = correctionVec * 0.5;
    p1->offsetPos(correctionVecHalf);
    p2->offsetPos(-correctionVecHalf);   
*/     
} 