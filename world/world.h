#ifndef WORLD_H
#define WORLD_H

#include "view.h"
#include "phys.h"
#include "object.h"
#include <vector>

class World
{
    public:
        World(bool withView, float fieldLength=40.0f, float fieldWidth=40.0f, float wallHeight=10.0f, float goalLength=2.0f, float goalWidth=6.5f, float goalHeight=3.0f, float goalThickness=0.5f, float playerRadius=0.3f, float ballRadius=0.5f, float playerMass=10.0f, float ballMass=5.0f);
        ~World();
        void setup(float fieldLength, float fieldWidth, float wallHeight, float goalLength, float goalWidth, float goalHeight, float goalThickness, float playerRadius, float ballRadius, float playerMass, float ballMass);
        int stepRealtime();
        int step(float delta);
        void playerAction(int playerNum, int action);
        bool getState(int playerNum, float* state, float& realReward, float& tempReward);
        void reset();
        
        View* view;
        Phys* phys;
        std::vector<Object*> objects;
        std::vector<Object*> dynamicObjects;
        Object* ball;
        Object* player1;
        Object* player2;
        Object* goalTrigger1;
        Object* goalTrigger2;
        float curTime;
        int withView;
};

#endif
