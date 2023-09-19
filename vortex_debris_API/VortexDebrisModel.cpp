#include "pch.h"
#include "VortexDebrisModel.h"
#include <iostream>

void VortexDebrisModel::updateSim() {
    //update simulation
    updateDebris();
    updateVortex();

    simTime += dt;
}


DebrisFlightParams VortexDebrisModel::runSimulation(){

    while (debrisPos.z >= 0 && simTime < MAX_FLIGHT_TIME) {
        updateSim();
    }

    double trajDist = hypot(debrisPos.x - startX, debrisPos.y);

    //valid trajectory?
    if (debrisState == FlightState::inAir && debrisFlightParams.traj_min < trajDist && trajDist < debrisFlightParams.traj_max) {
        
        debrisFlightParams.valid = true;
    }

    return debrisFlightParams;
}


Trajectory VortexDebrisModel::simulateTrajectory() {
    Trajectory traj = Trajectory();

    while (debrisPos.z >= 0 && simTime < MAX_FLIGHT_TIME) {
        traj.add(debrisPos);
        updateSim();
    }

    traj.add(debrisPos);

    return traj;
}

//compute debris euler time step
void VortexDebrisModel::updateDebris(){
    Vec3 debrisAcc = computeAcceleration();

    debrisPos += debrisVel * dt;

    debrisVel += debrisAcc * dt;

    switch (debrisState) {
        case onGround:
            if (debrisPos.z > 0.0) {
                debrisState = transitioning;
            }
            break;
        case transitioning:
            if (debrisPos.z >= 1.0) {
                debrisState = inAir;
            }
            break;
        case inAir:
            break;
    }
}

//compute vortex euler time step
void VortexDebrisModel::updateVortex(){

    vortexPosX += debrisFlightParams.vt * dt;
}
