#include "world/world.h"

int main(int argc, const char* argv[])
{
    World* world = new World(true);
    while (!world->stepRealtime()){
        float state[27];
        float reward, tempReward;
        bool done = world->getState(0, state, reward, tempReward);
        if (done) printf("Reward: %f\n", reward);
        for (int i=0; i<3; i++)
        {
            for (int h=0; h<3; h++)
            {
                for (int j=0; j<3; j++)
                {
                    printf("%f, ", state[3*(3*i+h)+j]);
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("--------------------------------\n");
        /*world->getState(1, state);
        for (int i=0; i<3; i++)
        {
            for (int h=0; h<3; h++)
            {
                for (int j=0; j<3; j++)
                {
                    printf("%f, ", state[3*(3*i+h)+j]);
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("*******************************\n");*/
    }
    delete world;
}
