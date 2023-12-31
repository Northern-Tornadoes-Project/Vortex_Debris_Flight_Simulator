#pragma once
#include <iostream>
#include <omp.h>
#include <vector>
#include <array>
#include <tuple>
#include <string>
#include <algorithm>
#include <memory>
#include "pch.h"
#include "BakerSterlingDebrisModel.h"
#include "RandomParameterGenerator.h"


#ifdef __cplusplus
	extern "C" {
#endif

#define VORTEX_DEBRIS_API __declspec(dllimport)

		double VORTEX_DEBRIS_API **matchTrajMonteCarlo(double params[]);
		double VORTEX_DEBRIS_API **getTraj(double params[]);
		double VORTEX_DEBRIS_API** getTrajRK4(double params[]);

#ifdef __cplusplus
}
#endif

	void unpackRandParams(double params[], RandomParameterGenerator& randParamGen);
	double* packResults(DebrisParams& dp, DebrisFlightParams& dfp);


