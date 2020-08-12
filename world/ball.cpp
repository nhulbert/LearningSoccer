#include "ball.h"
#include <glm/glm.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>

Ball::Ball(int withView, glm::mat4 inMat, glm::vec3 pos, float scale, int iterations, float mass)
{
    wv = withView;
    matrix = inMat;

    verticesSetup(pos, scale, iterations);
	if (withView)
	{
	    glSetup();
	}
	physSetup(scale, mass);
}

void Ball::verticesSetup(glm::vec3 pos, float scale, int iterations)
{
    int baseNumVertices = NUM_VERTICES = 24;
    for (int i=0; i<iterations; i++)
    {
        NUM_VERTICES *= 4;
    }
    
    int vecStride = 2;
    stride = vecStride*3;
    glm::vec3* vecs = new glm::vec3[NUM_VERTICES*vecStride];

    vecs[0] = glm::vec3(0.0f, 1.0f, 0.0f);
    vecs[vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);
    vecs[2*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);

    vecs[3*vecStride] = glm::vec3(0.0f, 1.0f, 0.0f);
    vecs[4*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);
    vecs[5*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);

    vecs[6*vecStride] = glm::vec3(0.0f, 1.0f, 0.0f);
    vecs[7*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);
    vecs[8*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);

    vecs[9*vecStride] = glm::vec3(0.0f, 1.0f, 0.0f);
    vecs[10*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);
    vecs[11*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);
    
    vecs[12*vecStride] = glm::vec3(0.0f, -1.0f, 0.0f);
    vecs[13*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);
    vecs[14*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);

    vecs[15*vecStride] = glm::vec3(0.0f, -1.0f, 0.0f);
    vecs[16*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);
    vecs[17*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);

    vecs[18*vecStride] = glm::vec3(0.0f, -1.0f, 0.0f);
    vecs[19*vecStride] = glm::vec3(sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);
    vecs[20*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);

    vecs[21*vecStride] = glm::vec3(0.0f, -1.0f, 0.0f);
    vecs[22*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, sqrt(2.0f)/2.0f);
    vecs[23*vecStride] = glm::vec3(-sqrt(2.0f)/2.0f, 0.0f, -sqrt(2.0f)/2.0f);

    int curpos = 0;
    int curpow = baseNumVertices;
    for (int i=baseNumVertices; i < NUM_VERTICES; i+=9)
    {
        glm::vec3 v1 = 
        (vecs[(curpos+1)*vecStride] - vecs[curpos*vecStride])/2.0f;
        glm::vec3 v2 = (vecs[(curpos+2)*vecStride] - vecs[curpos*vecStride])/2.0f;
        
        vecs[i*vecStride] = normalize(vecs[curpos*vecStride] + v1);
        vecs[(i+1)*vecStride] = normalize(vecs[curpos*vecStride] + 2.0f*v1);
        vecs[(i+2)*vecStride] = normalize(vecs[curpos*vecStride] + v1 + v2);
        
        vecs[(i+3)*vecStride] = normalize(vecs[curpos*vecStride] + v1 + v2);
        vecs[(i+4)*vecStride] = normalize(vecs[curpos*vecStride] + v2);
        vecs[(i+5)*vecStride] = normalize(vecs[curpos*vecStride] + v1);
        
        vecs[(i+6)*vecStride] = normalize(vecs[curpos*vecStride] + v2);
        vecs[(i+7)*vecStride] = normalize(vecs[curpos*vecStride] + v2 + v1);
        vecs[(i+8)*vecStride] = normalize(vecs[curpos*vecStride] + 2.0f*v2);
        
        vecs[(curpos+1)*vecStride] = normalize(vecs[curpos*vecStride] + v1);
        vecs[(curpos+2)*vecStride] = normalize(vecs[curpos*vecStride] + v2);
        
        curpos += 3;
        if (curpos == curpow)
        {
            curpos = 0;
            curpow *= 4;
        }
    }
    
    /*for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            printf("%f, ", matrix[i][j]);
        }
        printf("\n");
    }*/
    
    memcpy(&matrix[3][0], &pos, sizeof(glm::vec3));
    
    for (int i=0; i<NUM_VERTICES; i++)
    {
        vecs[i*vecStride+1] = vecs[i*vecStride];
        vecs[i*vecStride] *= scale;
    }
    
    vertices = (float*)vecs;
}

void Ball::physSetup(float scale, float inMass)
{
    collisionShape = new btSphereShape(scale);
    
    btTransform transform;
    for (int i=0; i<16; i++)
    {
        startMatrix[i] = ((float*)&matrix)[i];
    }
    transform.setFromOpenGLMatrix(startMatrix);
    btScalar mass(inMass);
    btVector3 localInertia(0, 0, 0);
    collisionShape->calculateLocalInertia(mass, localInertia);
    
	motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
	rigidBody = new btRigidBody(rbInfo);
}
