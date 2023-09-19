#pragma once

#include "VortexDebrisModel.h"
#include <array>

class BakerSterlingDebrisModel : public VortexDebrisModel
{
private:
	//baker sterling specific constants parameters
	static constexpr double a = 1.0;
	static constexpr double b = 1.0;
	static constexpr double c = 2.0;
	static constexpr double Er = 1.0 / 0.3;

	double K;
	double zm;

public:
	//initialize baker sterling model from debris parameters
	BakerSterlingDebrisModel(const DebrisParams& debParams, const DebrisFlightParams& debFlightParams) : VortexDebrisModel(debParams, debFlightParams) {

		K = (debrisFlightParams.s * pow(c, c / (a + 1))) / (pow(c - 1, (c - 1) / (a + 1)) * pow(log(2), c / (a + 1)));

		zm = debrisFlightParams.delta * debrisFlightParams.rm;
	}

	Vec3 computeAcceleration();
	std::array<const double, 3> computeFieldVel(const double r, const double z);
};

