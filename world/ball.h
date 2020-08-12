#ifndef BALL_H
#define BALL_H

#include "object.h"

class Ball : public Object
{
    public:
        Ball(int withView, glm::mat4 inMat, glm::vec3 pos, float scale, int iterations, float mass);
        void verticesSetup(glm::vec3 pos, float scale, int iterations);
        void physSetup(float scale, float mass);
};

#endif
