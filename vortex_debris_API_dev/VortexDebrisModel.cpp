#include "VortexDebrisModel.h"
#include <iostream>

DebrisFlightParams VortexDebrisModel::runSimulation(){

    //simulation loop
    while (debrisPos.z >= 0 && simTime < MAX_FLIGHT_TIME) {
        updateDebris();
        updateVortex();

        simTime += dt;
    }

    double trajDist = hypot(debrisPos.x - startX, debrisPos.y);

    //valid trajectory?
    if (debrisFlightParams.traj_min < trajDist && trajDist < debrisFlightParams.traj_max && launched) {
        
        debrisFlightParams.valid = true;
    }

    return debrisFlightParams;
}

//compute debris euler time step
void VortexDebrisModel::updateDebris(){
    Vec3 debrisAcc = computeAcceleration();

    debrisPos += debrisVel * dt;

    debrisVel += debrisAcc * dt;

    if (!launched && debrisPos.z > 0) {
        launched = true;
    }

    if (!inAir && debrisPos.z > 1) {
        inAir = true;
    }
}

//compute vortex euler time step
void VortexDebrisModel::updateVortex(){

    vortexPosX += debrisFlightParams.vt * dt;
}
