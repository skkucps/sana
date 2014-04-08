#include    <TMAC/Mobility/StationaryMobility.h>

Define_Module(StationaryMobility);

void StationaryMobility::initialize(int stage)
{
    srand((unsigned int)(time(NULL)));
    BaseMobility::initialize(stage);
    EV << "initializing IntersectionMobility stage " << stage << endl;
    if (stage == 0)
    {
    }
}


void StationaryMobility::initializePosition()
{
    move();
}

void StationaryMobility::move()
{
}
