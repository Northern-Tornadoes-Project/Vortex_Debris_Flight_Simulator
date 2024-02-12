#include "pch.h"
#include "VortexDebrisModel.h"
#include <iostream>

void VortexDebrisModel::updateSim() {
    //update simulation
    updateDebris();
    updateVortex();

    simTime += dt;
}

void VortexDebrisModel::updateSimRK4() {
    Vec3 initPos = debrisPos;
    Vec3 initVel = debrisVel;
    double initVortexPos = vortexPosX;

    //k1
    Vec3 k1 = computeAcceleration();
    debrisPos += debrisVel * (dt * 0.5);
    debrisVel += k1 * (dt * 0.5);
    vortexPosX += debrisFlightParams.vt * dt * 0.5;

    //k2
    Vec3 k2 = computeAcceleration();
    debrisVel = initVel + k2 * (dt * 0.5);

    //k3
    Vec3 k3 = computeAcceleration();
    debrisPos = initPos + debrisVel * dt;
    debrisVel = initVel + k3 * dt;
    vortexPosX = initVortexPos + debrisFlightParams.vt * dt;

    //k4
    Vec3 k4 = computeAcceleration();
    Vec3 k = Vec3();
    k = k1 + (k2 * 2.0);
    k += k4 + (k3 * 2.0);

    Vec3 debrisAcc = Vec3();
    debrisAcc = k * (1.0 / 6.0);
    debrisVel = initVel + debrisAcc * dt;

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

    if (heightState == notReached && debrisPos.z >= debrisFlightParams.req_height) {
        heightState = reached;
    }

    simTime += dt;
}


std::tuple<DebrisParams, DebrisFlightParams> VortexDebrisModel::runSimulation(){

    bool validFlight = false;

    while (simTime < MAX_FLIGHT_TIME) {
        updateSimRK4();

        if (debrisState == onGround && simTime > 0.1 * MAX_FLIGHT_TIME) {
            break;
        }

        if (debrisState == transitioning && debrisPos.z < 0.0) {
            break;
        }

        if (heightState == reached && debrisPos.z < debrisFlightParams.end_height) {
            validFlight = true;
            break;
        }
    }

    double trajDist = hypot(debrisPos.x - startX, debrisPos.y);

    //valid trajectory?
    if ((validFlight || (debrisState == transitioning && debrisFlightParams.traj_min <= 0.0)) && debrisFlightParams.traj_min < trajDist && trajDist < debrisFlightParams.traj_max) {
        
        debrisFlightParams.valid = true;
        debrisFlightParams.loftSpeed = computeGustSpeed(debrisFlightParams.loftPos.x, debrisFlightParams.loftPos.y, 3.0);
    }

    return { debrisParams, debrisFlightParams };
}


Trajectory VortexDebrisModel::simulateTrajectory() {
    Trajectory traj = Trajectory();

    while (simTime < MAX_FLIGHT_TIME) {
        traj.add(debrisPos);
        updateSim();

        if (heightState == reached && debrisPos.z < debrisFlightParams.end_height) {
            break;
        }
    }

    traj.add(debrisPos);

    return traj;
}

Trajectory VortexDebrisModel::simulateTrajectoryRK4() {
    Trajectory traj = Trajectory();

    while (simTime < MAX_FLIGHT_TIME) {
        traj.add(debrisPos);
        updateSimRK4();

        if (heightState == reached && debrisPos.z < debrisFlightParams.end_height) {
            break;
        }
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
