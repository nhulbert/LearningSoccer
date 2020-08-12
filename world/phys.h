#include <btBulletDynamicsCommon.h>
#include "object.h"
#include <vector>

struct GoalCollision {
    GoalCollision(btCollisionObject* o1, btCollisionObject* o2, int p) : object1(o1), object2(o2), player(p) {}
    btCollisionObject* object1;
    btCollisionObject* object2;
    int player;
};

class Phys
{
    public:
        Phys();
        ~Phys();
        void addObject(Object* object);
        void removeObject(Object* object);
        void registerGoalDetection(Object* o1, Object* o2, int player);
        void step(float delta);
        
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        btAlignedObjectArray<btCollisionShape*> collisionShapes;
        int gameState;
        std::vector<GoalCollision*> goalCollisions;
};
