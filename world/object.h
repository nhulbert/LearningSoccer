#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

class Object
{
	public:
	    Object(){}
	    Object(int withView, glm::mat4 inMat, glm::vec3 pos, glm::vec3 scale);
	    ~Object();
	    void draw(int shaderProgram, glm::mat4 view);
	    void verticesSetup(glm::vec3 pos, glm::vec3 scale);
	    void glSetup();
	    void physSetup(glm::vec3 scale);
        void applyForce(btVector3 vec);
        void getPosition(float* pos);
	    void getLinearVelocity(float* vel);
	    void getAngularVelocity(float* angVel);
	    void reset();
	    void setPosition(const float pos[3]);
	    
	    glm::mat4 matrix;
	    double startMatrix[16];
	    
	    int NUM_VERTICES;
	    
	    float* vertices;
	    int stride;
	    int wv;
	    unsigned int VBO, VAO;
	    btCollisionShape* collisionShape;
	    btDefaultMotionState* motionState;
        btRigidBody* rigidBody;
        btTriangleMesh* mesh;
};

#endif
