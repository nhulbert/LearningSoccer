#include "phys.h"

Phys::Phys()
{
    gameState = 0;
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

Phys::~Phys()
{
    for (GoalCollision* gc : goalCollisions)
    {
        delete gc;
    }
    delete dynamicsWorld;
    delete collisionConfiguration;
    delete dispatcher;
    delete overlappingPairCache;
    delete solver;
}

void Phys::addObject(Object* object)
{
    btRigidBody* body = object->rigidBody;
    dynamicsWorld->addRigidBody(body);
}

void Phys::removeObject(Object* object)
{
    btRigidBody* body = object->rigidBody;
    dynamicsWorld->removeRigidBody(body);
}

void Phys::registerGoalDetection(Object* o1, Object* o2, int player)
{
    goalCollisions.push_back(new GoalCollision(o1->rigidBody, o2->rigidBody, player));
    printf("registered\n");
}

void Phys::step(float delta)
{
    dynamicsWorld->stepSimulation(delta, 15, btScalar(1.0)/btScalar(300.0));
    
    int numManifolds = dispatcher->getNumManifolds();
    for (int i=0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
        btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
        btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());
        
        for (GoalCollision* gc : goalCollisions)
        {
            if ((gc->object1 == obA && gc->object2 == obB) || (gc->object1 == obB && gc->object2 == obA))
            {
                gameState = gc->player*2-1;
            }
        }
    }
}
