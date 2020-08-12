#include "world.h"
#include "ball.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <LinearMath/btVector3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const float WALL_THICKNESS = 10.0f;
const float FLOOR_THICKNESS = 1.0f;
const float GOALTRIGGER_THICKNESS = 0.1f;

World::World(bool wv, float fieldLength, float fieldWidth, float wallHeight, float goalLength, float goalWidth, float goalHeight, float goalThickness, float playerRadius, float ballRadius, float playerMass, float ballMass)
{
    withView = wv;
    view = nullptr;
    if (withView)
    {
        view = new View();
    }
    phys = new Phys();
    setup(fieldLength, fieldWidth, wallHeight, goalLength, goalWidth, goalHeight, goalThickness, playerRadius, ballRadius, playerMass, ballMass);
}

void World::setup(float fieldLength, float fieldWidth, float wallHeight, float goalLength, float goalWidth, float goalHeight, float goalThickness, float playerRadius, float ballRadius, float playerMass, float ballMass)
{
    for (int i=0; i<4; i++)
    {
        glm::mat4 objMat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 rawPos = glm::vec4(WALL_THICKNESS/2.0f, wallHeight/2.0f, -(fieldLength+WALL_THICKNESS)/2.0f, 1.0f);
        glm::vec3 pos = objMat * rawPos;
        objects.push_back(new Object(withView, objMat, pos, glm::vec3(fieldLength+WALL_THICKNESS, wallHeight, WALL_THICKNESS)));
    }
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(0.0f, -FLOOR_THICKNESS, 0.0f), glm::vec3(fieldLength*2.0f, FLOOR_THICKNESS, fieldLength*2.0f)));
    
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(-fieldLength/2.0f + goalThickness*2.0f, goalHeight/2.0f, -(goalWidth + goalThickness)/2.0f), glm::vec3(goalLength, goalHeight, goalThickness)));
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(-fieldLength/2.0f + goalThickness*2.0f, goalHeight/2.0f, (goalWidth + goalThickness)/2.0f), glm::vec3(goalLength, goalHeight, goalThickness)));
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(fieldLength/2.0f - goalThickness*2.0f, goalHeight/2.0f, -(goalWidth + goalThickness)/2.0f), glm::vec3(goalLength, goalHeight, goalThickness)));
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(fieldLength/2.0f - goalThickness*2.0f, goalHeight/2.0f, (goalWidth + goalThickness)/2.0f), glm::vec3(goalLength, goalHeight, goalThickness)));
    
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(-fieldLength/2.0f + goalThickness*2.0f, goalHeight + (goalThickness/2.0f), 0.0f), glm::vec3(goalLength, goalThickness, goalWidth + 2.0f*goalThickness)));
    objects.push_back(new Object(withView, glm::mat4(1.0f), glm::vec3(fieldLength/2.0f - goalThickness*2.0f, goalHeight + (goalThickness/2.0f), 0.0f), glm::vec3(goalLength, goalThickness, goalWidth + 2.0f*goalThickness)));
    
    goalTrigger1 = new Object(withView, glm::mat4(1.0f), glm::vec3((-fieldLength + GOALTRIGGER_THICKNESS)/2.0f, goalHeight/2.0f, 0.0f), glm::vec3(GOALTRIGGER_THICKNESS, goalHeight, goalWidth));
    objects.push_back(goalTrigger1);
    
    goalTrigger2 = new Object(withView, glm::mat4(1.0f), glm::vec3((fieldLength - GOALTRIGGER_THICKNESS)/2.0f, goalHeight/2.0f, 0.0f), glm::vec3(GOALTRIGGER_THICKNESS, goalHeight, goalWidth));
    objects.push_back(goalTrigger2);
    
    ball = new Ball(withView, glm::mat4(1.0f), glm::vec3(0.0f, ballRadius, 0.0f), ballRadius, 3, 5.0f);
    objects.push_back(ball);
    dynamicObjects.push_back(ball);
    
    player1 = new Ball(withView, glm::mat4(1.0f), glm::vec3(-fieldLength/2.0f + (goalLength*2.0f), playerRadius, 0.0f), playerRadius, 3, playerMass);
    objects.push_back(player1);
    dynamicObjects.push_back(player1);
    
    player2 = new Ball(withView, glm::mat4(1.0f), glm::vec3(fieldLength/2.0f - (goalLength*2.0f), playerRadius, 0.0f), playerRadius, 3, playerMass);
    objects.push_back(player2);
    dynamicObjects.push_back(player2);
    
    for (Object* obj : objects)
    {
        phys->addObject(obj);
    }
    
    phys->registerGoalDetection(ball, goalTrigger2, 0);
    phys->registerGoalDetection(ball, goalTrigger1, 1);
    
    curTime = glfwGetTime();
}

World::~World()
{
    delete phys;
    while (objects.size())
    {
        delete objects.back();
        objects.pop_back();
    }
    delete view;
}

int World::stepRealtime()
{
    float prevTime = curTime;
    curTime = glfwGetTime();
    float delta = curTime - prevTime;
    
    return step(delta);
}

int World::step(float delta)
{
    phys->step(delta);
    
    /*if (phys->gameState != 0)
    {
        //printf("Player %d scores\n", phys->gameState);
        reset();
    }*/

    for (Object* obj : objects)
    {
        btTransform trans;
        obj->rigidBody->getMotionState()->getWorldTransform(trans);
        
        double tempArr[16];
        trans.getOpenGLMatrix(tempArr);
        for (int i=0; i<16; i++)
        {
            ((float*)&(obj->matrix))[i] = (float)tempArr[i];
        }
    }
    
    int toExit = false;
    
    if (withView)
    {
        int pressedKey;
        toExit = view->displayView(&objects, delta, &pressedKey);
        
        if (pressedKey & 1)
        {
            playerAction(0, 1);
        }
        else if (pressedKey & 2)
        {
            playerAction(0, 2);
        }
        else if (pressedKey & 4)
        {
            playerAction(0, 3);
        }
        else if (pressedKey & 8)
        {
            playerAction(0, 4);
        }

    }
    
    return toExit;
}

void World::playerAction(int playerNum, int action)
{
    Object* players[] = {player1, player2};
    Object* player = players[playerNum];
    
    btVector3 moveDir(0.0, 0.0, 0.0);
    
    switch(action)
    {
        case 1:
            moveDir += btVector3(1.0, 0.0, 0.0);
            break;
        case 2:
            moveDir += btVector3(-1.0, 0.0, 0.0);
            break;
        case 3:
            moveDir += btVector3(0.0, 0.0, -1.0);
            break;
        case 4:
            moveDir += btVector3(0.0, 0.0, 1.0);
            break;
    }
    
    btVector3 ballDir = ball->rigidBody->getCenterOfMassPosition() - player->rigidBody->getCenterOfMassPosition();
    ballDir[1] = 0.0;
    ballDir.normalize();
    
    btVector3 forceDir = (moveDir[0] * ballDir) + (moveDir[2] * btVector3(-ballDir[2], 0, ballDir[0]));
    
    
    player->applyForce(forceDir);
}

bool World::getState(int playerNum, float* state, float& realReward, float& tempReward)
{
    Object* players[] = {player1, player2};
    Object* player = players[playerNum];
    Object* othPlayer = players[1-playerNum];
    
    Object* stateObjs[] = {player, othPlayer, ball};
    
    for (int i=0; i<3; i++)
    {
        Object* stateObj = stateObjs[i];
        
        stateObj->getPosition(&state[i*9]);
        stateObj->getLinearVelocity(&state[i*9+3]);
        stateObj->getAngularVelocity(&state[i*9+6]);
        
        if (playerNum)
        {
            for (int h=0; h<3; h++)
            {
                state[i*9+h*3] *= -1.0f;
            }
        }
    }
    
    bool done = (phys->gameState != 0);
    realReward = (float)((playerNum*2-1)*phys->gameState);
    
    Object* goalTriggers[] = {player1, player2};
    glm::vec3 goalPos, ballPos;
    goalTriggers[playerNum]->getPosition((float*)&goalPos);
    ball->getPosition((float*)&ballPos);
    
    tempReward = (1.0f / glm::dot(goalPos-ballPos, goalPos-ballPos))/1000000.0f;
    //printf("TEMP REWARD: %f\n", tempReward);
    
    phys->gameState = 0;
    return done;
}

void World::reset()
{
    std::vector<float*> resetObjs;

    for (Object* obj : dynamicObjects)
    {
        phys->removeObject(obj);
        
        float* coords = new float[3];
        bool conflicting;
        do
        {
            conflicting = false;
            for (int i=0; i<3; i+=2)
            {
                coords[i] = ((rand()%100000)/100000.0f - 0.5f)*33.0f;
            }
            coords[1] = 1.0f;
            
            for (int i=0; i<resetObjs.size(); i++)
            {
                float accum = 0;
                for (int j=0; j<3; j++)
                {
                    float diff = coords[j] - resetObjs[i][j];
                    accum += diff*diff;
                }
                
                if (sqrt(accum) < 0.7f)
                {
                    conflicting = true;
                    break;
                }
            }
        }
        while (conflicting);
        resetObjs.push_back(coords);
        
        obj->setPosition(coords);
        phys->addObject(obj);
        float xdir = ((rand()%100000)/100000.0f - 0.5f)*50.0f;
        float zdir = ((rand()%100000)/100000.0f - 0.5f)*50.0f;
        obj->applyForce(btVector3(xdir, 0.0, zdir));
    }
    
    while (resetObjs.size())
    {
        delete [] resetObjs.back();
        resetObjs.pop_back();
    }
}
