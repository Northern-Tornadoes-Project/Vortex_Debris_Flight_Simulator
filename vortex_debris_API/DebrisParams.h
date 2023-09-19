#pragma once

//class for storing and handling the debris object parameter initialization
class DebrisParams{
public:

	double l;				// Characteristic Length of Debris
	double rho_m;			// Density of Debris
	double z_correction;	//debris z height correction factor (center of mass?)

	DebrisParams() {
		l = 0;
		rho_m = 0;
		z_correction = 0;
	}

	DebrisParams(double* params) {
		l = params[0];
		rho_m = params[1];
		z_correction = params[2];
	}

};

