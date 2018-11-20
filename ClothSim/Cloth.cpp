#include "pch.h"
#include "Cloth.h"

Cloth::Cloth()
{
}


Cloth::~Cloth()
{
}

Cloth::Cloth(float clothWidth, float clothHeight, float particleMass, int columns, int rows, float iterations, float damp, float t) {
	numClothColumns = columns;
	numClothRows = rows;
    constraintIterations = iterations;
    dampening = damp;
    timeUnit = t;    

    particles.resize(numClothColumns * numClothRows);


    // Creating particles in a grid of particles from (0,0,0) to (width,-height,0)
    // we are translating each particle's grid position in the cloth to it's actual vector position in the coordinate plane.
    for (int x = 0; x < numClothColumns; x++) {
        for (int y = 0; y < numClothRows; y++) {
            vmath::vec3 pos = vmath::vec3(
                clothWidth * (x / (float)numClothColumns),
                -clothHeight * (y / (float)numClothRows),
                0
            );
            particles[(y * numClothColumns) + x] = Particle(pos, particleMass);
        }
    }

    // Constraints made here
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

    for (int i = 0; i < 3; i++) {
        getParticle(0 + i, 0)->offsetPos(vmath::vec3(0.5, 0.0, 0.0));
        getParticle(0 + i, 0)->setMovable(false);

        getParticle(numClothColumns - 1 - i, 0)->offsetPos(vmath::vec3(-0.5, 0.0, 0.0));
        getParticle(numClothColumns - 1 - i, 0)->setMovable(false);        
    }
}

/*
//Load BMP and bind it into openGL
GLuint Cloth::loadCustomBMP(const char * imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("Image could not be opened\n"); return 0; }

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
*/

//texCoords are given from drawShaded()
//They are given in CW order at the moment, I'm not sure if that is correct.
void Cloth::drawTriangle(Particle* p1, Particle* p2, Particle* p3, vmath::vec2 v1, vmath::vec2 v2, vmath::vec2 v3) {

	glColor3f(1.0f, 1.0f, 1.0f);

	//glTexCoord2f((GLfloat) v1[0], (GLfloat) v1[1]);
    glNormal3fv((GLfloat *) &(vmath::normalize(p1->getNormal())));
    glVertex3fv((GLfloat *) &(p1->getPos()));

	//glTexCoord2f((GLfloat) v2[0], (GLfloat) v2[1]);
    glNormal3fv((GLfloat *) &(vmath::normalize(p2->getNormal())));
    glVertex3fv((GLfloat *) &(p2->getPos()));

	//glTexCoord2f((GLfloat) v3[0], (GLfloat) v3[1]);
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


		//loadCustomBMP("flag.bmp");
        glBegin(GL_TRIANGLES);
        for (int x = 0; x < numClothColumns - 1; x++) {
            for (int y = 0; y < numClothRows - 1; y++) {
                vmath::vec3 color = vmath::vec3(0.0f, 0.0f, 0.0f);
                if (x % 2) { // if x is odd
                    color = vmath::vec3(0.6f, 0.2f, 0.2f);                    
                } else {
					color = vmath::vec3(1.0f, 1.0f, 1.0f);
                }

				float startX = x/numClothColumns;
				float endX = (x + 1) / numClothColumns;
				float startY = y / numClothRows;
				float endY = (y + 1) / numClothRows;

                drawTriangle(
                    getParticle(x + 1, y),
                    getParticle(x, y),
                    getParticle(x, y + 1),
                    vmath::vec2(startX, startY),
					vmath::vec2(endX, startY),
					vmath::vec2(startX, endY)
                );
                drawTriangle(
                    getParticle(x + 1, y + 1),
                    getParticle(x + 1, y),
                    getParticle(x, y + 1),
					vmath::vec2(endX, startY),
					vmath::vec2(startX, endY),
					vmath::vec2(endX, endY)
                );        
            }
        }
        glEnd();                                    
    }
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