#pragma once
#include <string>
#include "Vec3.h"

//class for storing and handling the debris flight parameter initialization
class DebrisFlightParams{

public:
	static constexpr double g = 9.81;	// m/s^2 Acceleration due to Gravity
	static constexpr double rho = 1.23; // kg/m^3 Air Density @ Sea  Level

	double width;		// damage width
	double rm;			// Core Radius
	double s;			// Swirl Ratio
	double delta;		// Terrain Roughness
	double vt;			// Translational Speed
	double vr;			// Max Radial Speed
	double cd_sm;		// Drag Coeffcient
	double cd_air;		// Drag Coeffcient
	double cl;			// Drag Coeffcient
	double traj_min;    // min / max trajectories distances
	double traj_max;

	double loftSpeed = 0.0;
	Vec3 loftPos;
	bool valid = false;
	
	DebrisFlightParams() {
		width = 0;
		rm = 0;
		s = 0;
		delta = 0;
		vt = 0;
		vr = 0;
		cd_sm = 0;
		cd_air = 0;
		cl = 0;
		traj_min = 0;
		traj_max = 0;
	}

	DebrisFlightParams(double* params) {
		width = params[0];
		rm = params[1];
		s = params[2];
		delta = params[3];
		vt = params[4];
		vr = params[5];
		cd_sm = params[6];
		cd_air = params[7];
		cl = params[8];
		traj_min = params[9];
		traj_max = params[10];
	}
};

