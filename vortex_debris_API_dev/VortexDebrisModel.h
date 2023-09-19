#pragma once

#include <vector>

#include "Vec3.h"
#include "DebrisParams.h"
#include "DebrisFlightParams.h"

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
	bool inAir = false;
	bool launched = false;

	DebrisParams debrisParams;
	DebrisFlightParams debrisFlightParams;


public:

	//initalize simulation here
	VortexDebrisModel(const DebrisParams &debParams, const DebrisFlightParams &debFlightParams) {

		this->debrisParams = debParams;
		this->debrisFlightParams = debFlightParams;

		MAX_FLIGHT_TIME = debrisFlightParams.rm * 10.0 / debrisFlightParams.vt; //time for vortex to move 10 * rm

		k = debrisFlightParams.rho / (2 * debrisParams.rho_m * debrisParams.l);

		debrisVel = Vec3();

		startX = debrisFlightParams.rm * -1.0;

		debrisPos = Vec3(startX, 0.0, 0.0);

		//vortex starts centered at the origin, move debris around this
		vortexPosX = 0.0;
		simTime = 0.0;
	}

	DebrisFlightParams runSimulation();
	void updateDebris();
	void updateVortex();
	virtual Vec3 computeAcceleration() = 0;
};

