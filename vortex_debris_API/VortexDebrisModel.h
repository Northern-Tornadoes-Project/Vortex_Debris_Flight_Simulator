#pragma once

#include <vector>
#include <tuple>

#include "Vec3.h"
#include "DebrisParams.h"
#include "DebrisFlightParams.h"
#include "Trajectory.h"

//abstract class for simulating debris flights
class VortexDebrisModel
{
protected:
	static constexpr double dt = 0.001;
	double MAX_FLIGHT_TIME;

	double k; // from Holmes (2003) - Buoyancy Parameter

	Vec3 debrisVel;
	Vec3 debrisPos;
	double startX;
	double vortexPosX;
	double simTime;

	DebrisParams debrisParams;
	DebrisFlightParams debrisFlightParams;

	enum DebrisState {
		onGround = 0,
		transitioning = 1,
		inAir = 2,
	};

	enum HeightState {
		notReached = 0,
		reached = 1,
	};

	DebrisState debrisState;
	HeightState heightState;


public:

	//initalize simulation here
	VortexDebrisModel(const DebrisParams &debParams, const DebrisFlightParams &debFlightParams) {

		this->debrisParams = debParams;
		this->debrisFlightParams = debFlightParams;

		MAX_FLIGHT_TIME = debrisFlightParams.rm * 10.0 / debrisFlightParams.vt; //time for vortex to move 10 * rm

		k = debrisFlightParams.rho / (2 * debrisParams.rho_m * debrisParams.l);

		debrisState = onGround;
		heightState = notReached;

		debrisVel = Vec3();

		startX = debrisFlightParams.rm * -1.0;

		debrisPos = Vec3(startX, 0.0, 0.0);

		//vortex starts centered at the origin, move debris around this
		vortexPosX = 0.0;
		simTime = 0.0;
	}

	std::tuple<DebrisParams, DebrisFlightParams> runSimulation();
	Trajectory simulateTrajectory();
	Trajectory simulateTrajectoryRK4();
	void updateSim();
	void updateSimRK4();
	void updateDebris();
	void updateVortex();
	virtual Vec3 computeAcceleration() = 0;
	virtual double computeGustSpeed(const double x, const double y, double gustTime) = 0;
};

