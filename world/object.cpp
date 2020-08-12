#include "object.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

Object::Object(int withView, glm::mat4 inMat, glm::vec3 pos, glm::vec3 scale)
{
    wv = withView;
    NUM_VERTICES = 36;
    
    stride = 6;
    vertices = new float[NUM_VERTICES*stride];

    matrix = inMat;
    
	verticesSetup(pos, scale);
	if (withView)
	{
	    glSetup();
    }
	physSetup(scale);
}

Object::~Object()
{
    if (wv)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    delete [] vertices;
    delete collisionShape;
    delete motionState;
    delete rigidBody;
}

void Object::verticesSetup(glm::vec3 pos, glm::vec3 scale)
{
        /*for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            printf("%f, ", matrix[i][j]);
        }
        printf("\n");
    }*/
    
    memcpy(&matrix[3][0], &pos, sizeof(glm::vec3));
	
    float distinctVertices[] =
    {
        scale.x*0.5f, scale.y*0.5f, scale.z*-0.5f,
        scale.x*-0.5f, scale.y*0.5f, scale.z*-0.5f,
        scale.x*-0.5f, scale.y*-0.5f, scale.z*-0.5f,
        scale.x*0.5f, scale.y*-0.5f, scale.z*-0.5f,
        
        scale.x*0.5f, scale.y*0.5f, scale.z*0.5f,
        scale.x*-0.5f, scale.y*0.5f, scale.z*0.5f,
        scale.x*-0.5f, scale.y*-0.5f, scale.z*0.5f,
        scale.x*0.5f, scale.y*-0.5f, scale.z*0.5f
    };
    
    int indices[] =
    {
        0, 1, 2,
        2, 3, 0,
        
        6, 5, 4,
        4, 7, 6,
        
        0, 3, 7,
        7, 4, 0,
        
        1, 5, 6,
        6, 2, 1,
        
        0, 4, 5,
        5, 1, 0,
        
        3, 2, 6,
        6, 7, 3
    };
    
    for (int i=0; i<NUM_VERTICES; i++)
    {
        int vertexNum = i*stride;
        for (int j=0; j<3; j++)
        {
            vertices[vertexNum + j] = distinctVertices[indices[i] * 3 + j];
        }
        if (i % 3 == 2)
        {
            glm::vec3 ps[3];
            for (int j=0; j<3; j++)
            {
                ps[j] = glm::vec3(vertices[vertexNum-j*stride], vertices[vertexNum-j*stride+1], vertices[vertexNum-j*stride+2]);
            }
            glm::vec3 v1 = ps[0]-ps[1];
            glm::vec3 v2 = ps[1]-ps[2];
            glm::vec3 res = glm::normalize(glm::cross(v1, v2));
            for (int j=0; j<3; j++)
            {
                vertices[vertexNum-j*stride+3] = res.x;
                vertices[vertexNum-j*stride+4] = res.y;
                vertices[vertexNum-j*stride+5] = res.z;
            }
        }
    }
}

void Object::glSetup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NUM_VERTICES*stride, vertices, GL_STATIC_DRAW);

    /*for (int i=0; i<NUM_VERTICES*6; i++)
    {
        printf("Vertex coord %f\n", vertices[i]);
        if (i % 6 == 5) {
            printf("\n");
        }
    }*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0);
}

void Object::physSetup(glm::vec3 scale)
{
    mesh = new btTriangleMesh();

    for (int i=0; i<NUM_VERTICES; i+=3)
    {
        btVector3 points[3];
        for (int h=0; h<3; h++)
        {
            int ind = (i+h)*stride;
            points[h] = btVector3(vertices[ind], vertices[ind+1], vertices[ind+2]);
        }
        mesh->addTriangle(points[0], points[1], points[2], true);
    }    
    
    collisionShape = new btBvhTriangleMeshShape(mesh, false);
    
    btTransform transform;
    for (int i=0; i<16; i++)
    {
        startMatrix[i] = ((float*)&matrix)[i];
    }
    transform.setFromOpenGLMatrix(startMatrix);
    btScalar mass(0.0);
    btVector3 localInertia(0, 0, 0);
    
	motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
	rigidBody = new btRigidBody(rbInfo);
}

void Object::draw(int shaderProgram, glm::mat4 view)
{
    glm::mat4 modelview = view * matrix;

    int modelLoc = glGetUniformLocation(shaderProgram, "modelview");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelview));
    
    int normMatLoc = glGetUniformLocation(shaderProgram, "normMat");
    glUniformMatrix3fv(normMatLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(modelview)))));
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);    
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
}

void Object::applyForce(btVector3 vec)
{
    if (vec.length2() > 0.0001)
    {    
        vec *= 100;
        rigidBody->applyCentralForce(vec);
        rigidBody->activate();
    }
}

void Object::getPosition(float* pos)
{
    btVector3 vecPos = rigidBody->getCenterOfMassPosition();
    
    pos[0] = vecPos[0]/30.0f;
    pos[1] = vecPos[1]/30.0f;
    pos[2] = vecPos[2]/30.0f;
}

void Object::getLinearVelocity(float* vel)
{
    btVector3 vecVel = rigidBody->getLinearVelocity();
    
    vel[0] = vecVel[0]/30.0f;
    vel[1] = vecVel[1]/30.0f;
    vel[2] = vecVel[2]/30.0f;
}

void Object::getAngularVelocity(float* angVel)
{
    btVector3 vecAngVel = rigidBody->getAngularVelocity();
    
    angVel[0] = vecAngVel[0]/100.0f;
    angVel[1] = vecAngVel[1]/100.0f;
    angVel[2] = vecAngVel[2]/100.0f;
}

void Object::reset()
{
    btTransform transform;
    transform.setFromOpenGLMatrix(startMatrix);
    
    btScalar mass = rigidBody->getMass();
    btVector3 inertia = rigidBody->getLocalInertia();
    
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    rigidBody->setMassProps(0.0, inertia);
    
    rigidBody->setWorldTransform(transform);
    rigidBody->setInterpolationWorldTransform(transform);
    rigidBody->setCenterOfMassTransform(transform);
    rigidBody->setLinearVelocity(btVector3(0,0,0));
    rigidBody->setAngularVelocity(btVector3(0,0,0));
	motionState->setWorldTransform(transform);
	
	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	rigidBody->setMassProps(mass, inertia);
	rigidBody->activate();
}

void Object::setPosition(const float pos[3])
{
    btTransform transform;
    transform.setFromOpenGLMatrix(startMatrix);
    transform.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    
    btScalar mass = rigidBody->getMass();
    btVector3 inertia = rigidBody->getLocalInertia();
    
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    rigidBody->setMassProps(0.0, inertia);
    
    rigidBody->setWorldTransform(transform);
    rigidBody->setInterpolationWorldTransform(transform);
    rigidBody->setCenterOfMassTransform(transform);
    rigidBody->setLinearVelocity(btVector3(0,0,0));
    rigidBody->setAngularVelocity(btVector3(0,0,0));
	motionState->setWorldTransform(transform);
	
	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	rigidBody->setMassProps(mass, inertia);
	rigidBody->activate();
}
