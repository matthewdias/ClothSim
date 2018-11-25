#include "pch.h"
#include "Cloth.h"

Cloth::Cloth()
{
}


Cloth::~Cloth()
{
}

Cloth::Cloth(float clothWidth, float clothHeight, float particleMass, int columns, int rows, float iterations, float damp, float t, vmath::vec3 givenPos, bool vert) {
	numClothColumns = columns;
	numClothRows = rows;
    constraintIterations = iterations;
    dampening = damp;
    timeUnit = t;    

    particles.resize(numClothColumns * numClothRows);

    // Creating particles in a grid of particles from givenPos to givenPos + (width,-height,0)
	// we are translating each particle's grid position in the cloth to it's actual vector position in the coordinate plane.
	for (int x = 0; x < numClothColumns; x++) {
		for (int y = 0; y < numClothRows; y++) {
			vmath::vec3 pos;
			if (!vert) {
				pos = vmath::vec3(
					clothWidth * (x / (float)numClothColumns),
					0,
					-clothHeight * (y / (float)numClothRows)
				);
			} else {
				pos = vmath::vec3(
					clothWidth * (x / (float)numClothColumns),
					-clothHeight * (y / (float)numClothRows),
					0
					
				);
			}
			pos = givenPos + pos;
			particles[(y * numClothColumns) + x] = Particle(pos, particleMass);
		}
	}

	for (int x = 0; x < numClothColumns; x++) {
		for (int y = 0; y < numClothRows; y++) {
			if (x < numClothColumns - 1)
				makeConstraint(getParticle(x, y), getParticle(x + 1, y));
			if (y < numClothRows - 1)
				makeConstraint(getParticle(x, y), getParticle(x, y + 1));
			if (x < numClothColumns - 1 && y < numClothRows - 1) {
				makeConstraint(getParticle(x, y), getParticle(x + 1, y + 1));
				makeConstraint(getParticle(x + 1, y), getParticle(x, y + 1));
			}
		}
	}

    for (int x = 0; x < numClothColumns; x++) {
        for (int y = 0; y < numClothRows; y++) {
            if (x < numClothColumns - 2)
                makeConstraint(getParticle(x, y), getParticle(x + 2, y));
            if (y < numClothRows - 2)
                makeConstraint(getParticle(x, y), getParticle(x, y + 2));
            if (x < numClothColumns - 2 && y < numClothRows - 2) {
                makeConstraint(getParticle(x, y), getParticle(x + 2, y + 2));
                makeConstraint(getParticle(x + 2, y), getParticle(x, y + 2));
            }
        }
    }
}

//texCoords are given from drawShaded()
void Cloth::drawTriangle(Particle* p1, Particle* p2, Particle* p3, vmath::vec2 v1, vmath::vec2 v2, vmath::vec2 v3) {
	glTexCoord2f(v1[0], v1[1]);
	glNormal3fv((GLfloat *) &(vmath::normalize(p1->getNormal())));
	glVertex3fv((GLfloat *) &(p1->getPos()));

	glTexCoord2f(v2[0], v2[1]);
	glNormal3fv((GLfloat *) &(vmath::normalize(p2->getNormal())));
	glVertex3fv((GLfloat *) &(p2->getPos()));

	glTexCoord2f(v3[0], v3[1]);
    glNormal3fv((GLfloat *) &(vmath::normalize(p3->getNormal())));
    glVertex3fv((GLfloat *) &(p3->getPos()));
}

void Cloth::drawShaded() {
    std::vector<Particle>::iterator pIterator;
    for (pIterator = particles.begin(); pIterator != particles.end(); pIterator++) {
        (*pIterator).resetNormal();
    }

    for (int x = 0; x < numClothColumns - 1; x++) {
        for (int y = 0; y < numClothRows - 1; y++) {
            vmath::vec3 normal = calcTriangleNormal(
                getParticle(x + 1, y),
                getParticle(x, y),
                getParticle(x, y + 1)
            );

            getParticle(x + 1, y)->addToNormal(normal);
            getParticle(x, y)->addToNormal(normal);
            getParticle(x, y + 1)->addToNormal(normal);

            normal = calcTriangleNormal(
                getParticle(x + 1, y + 1),
                getParticle(x + 1, y),
                getParticle(x, y + 1)
            );

            getParticle(x + 1, y + 1)->addToNormal(normal);
            getParticle(x + 1, y)->addToNormal(normal);
            getParticle(x, y + 1)->addToNormal(normal);                            
        }                                  
    }


	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	for (float x = 0; x < numClothColumns - 1; x++) {
		for (float y = 0; y < numClothRows - 1; y++) {
			float startX = x / numClothColumns;
			float endX = (x + 1) / numClothColumns;
			float startY = 1 - (y / numClothRows);
			float endY = 1 - ((y + 1) / numClothRows);

			//printf("%4.2f | %4.2f | %4.2f | %4.2f\n", startX, endX, startY, endY);
			
			drawTriangle(
				getParticle(x + 1, y),
				getParticle(x, y),
				getParticle(x, y + 1),
				vmath::vec2(endX, startY),
				vmath::vec2(startX, startY),
				vmath::vec2(startX, endY)
			);
			
			drawTriangle(
				getParticle(x + 1, y + 1),
				getParticle(x + 1, y),
				getParticle(x, y + 1),
				vmath::vec2(endX, endY),
				vmath::vec2(endX, startY),
				vmath::vec2(startX, endY)
			);
			
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
}

void Cloth::timeStep() {
    std::vector<Constraint>::iterator c;
    for(int i=0; i<constraintIterations; i++)
    {
        for(c=constraints.begin(); c != constraints.end(); c++)
        {
            (*c).satisfyConstraint();
        }
    }

    std::vector<Particle>::iterator p;
    for(p = particles.begin(); p != particles.end(); p++)
    {
        (*p).timeStep(dampening, timeUnit);
    }

    for(int i=0; i<constraintIterations; i++)
    {
        for(c=constraints.begin(); c != constraints.end(); c++)
        {
            vmath::vec3 differenceVec = (*c).p2->getPos() - (*c).p1->getPos();
            float currDistance = (float)vmath::length(differenceVec);
            vmath::vec3 correctionVec = differenceVec * (1 - (*c).restDistance/currDistance);
            vmath::vec3 correctionVecHalf = correctionVec * 0.5;
			(*c).p1->offsetPos(correctionVecHalf);
			(*c).p2->offsetPos(-correctionVecHalf);
        }
    }
}                                  

void Cloth::addForce(vmath::vec3 dir) {
    dir = dir * timeUnit * timeUnit; 
    std::vector<Particle>::iterator pIterator;
    for (pIterator = particles.begin(); pIterator != particles.end(); pIterator++) {
        (*pIterator).addForce(dir);
    }
}

void Cloth::windForce(vmath::vec3 dir) {
    dir = dir * timeUnit * timeUnit;
    for(int x = 0; x < numClothColumns - 1; x++){
        for(int y = 0; y < numClothRows - 1; y++){
            addWindPerTriangle(getParticle(x+1, y), getParticle(x,y), getParticle(x,y+1), dir);
            addWindPerTriangle(getParticle(x+1, y+1), getParticle(x+1, y), getParticle(x, y+1), dir);                                
        }
    }
}

vmath::vec3 Cloth::calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3) {
    //Gets positions of particles
    vmath::vec3 pos1 = p1->getPos();
    vmath::vec3 pos2 = p2->getPos();
    vmath::vec3 pos3 = p3->getPos();

    vmath::vec3 v1 = pos2-pos1;
    vmath::vec3 v2 = pos3-pos1;

	return vmath::cross(v1, v2);
}

void Cloth::addWindPerTriangle(Particle *p1, Particle *p2, Particle *p3, const vmath::vec3 dir) {
	vmath::vec3 norm = calcTriangleNormal(p1, p2, p3);
	vmath::vec3 d = vmath::normalize(norm);
	vmath::vec3 force = norm * (vmath::dot(d, dir));
	
    p1->addForce(force);
    p2->addForce(force);
    p3->addForce(force);
}

void Cloth::setMass(float mass) {
	for (int x = 0; x < numClothColumns; x++) {
		for (int y = 0; y < numClothRows; y++) {
			particles[(y * numClothColumns) + x].setMass(mass);
		}
	}
}

/*
    x = column
    y = row
*/
Particle* Cloth::getParticle(int x, int y) {
    return &particles[(y * numClothColumns) + x];
}

void Cloth::makeConstraint(Particle *p1, Particle *p2) {
    constraints.push_back(Constraint(p1,p2));
}

void Cloth::ballCollision(const vmath::vec3 center, const float radius) {
    std::vector<Particle>::iterator pIterator;
	for (pIterator = particles.begin(); pIterator != particles.end(); pIterator++) {
		vmath::vec3 distVector = (*pIterator).getPos() - center;
		float distance = (float)vmath::length(distVector);

        if (distance < radius) {
            float offset = radius - distance;
            vmath::vec3 n = vmath::normalize(distVector);            
            (*pIterator).offsetPos(n * offset);
        }
	}
}

void Cloth::planeCollision(const float yValue) {
	std::vector<Particle>::iterator pIterator;
	for (pIterator = particles.begin(); pIterator != particles.end(); pIterator++) {
		float dist = (*pIterator).getPos()[1] - yValue;

		if ((*pIterator).getPos()[1] < yValue) {
			float offset = -dist + 0.01;
			vmath::vec3 n = vmath::normalize(vmath::vec3(0, 1, 0));
			(*pIterator).offsetPos(n * offset);
		}
	}
}