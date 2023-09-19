#pragma once
#include <array>
#include <random>
#include <time.h>
#include "DebrisFlightParams.h";


//class for randomly initalizing debris flight parameters (currently header only)
class RandomParameterGenerator
{
private:
	std::default_random_engine generator;

public:
	std::array<double, 3> rm;
	std::array<double, 3> vt;
	std::array<double, 3> vr;
	std::array<double, 3> delta;
	std::array<double, 3> s;
	std::array<double, 3> cd_sm;
	std::array<double, 3> cd_air;
	std::array<double, 3> cl;

	RandomParameterGenerator() {
		rm = { 0.0, 0.0, 0.0 };
		vt = { 0.0, 0.0, 0.0 };
		vr = { 0.0, 0.0, 0.0 };
		delta = { 0.0, 0.0, 0.0 };
		s = { 0.0, 0.0, 0.0 };
		cd_sm = { 0.0, 0.0, 0.0 };
		cd_air = { 0.0, 0.0, 0.0 };
		cl = { 0.0, 0.0, 0.0 };
		generator.seed(time(NULL));
	}

	RandomParameterGenerator(double* params) {
		rm = { params[0], params[1], params[2]};
		vt = { params[3], params[4], params[5] };
		vr = { params[6], params[7], params[8] };
		delta = { params[9], params[10], params[11] };
		s = { params[12], params[13], params[14] };
		cd_sm = { params[15], params[16], params[17] };
		cd_air = { params[18], params[19], params[20] };
		cl = { params[21], params[22], params[22] };
		generator.seed(time(NULL));
	}

	// TODO: refactor lol
	void generate(DebrisFlightParams& dfp) {

		dfp.rm = rm[2] >= 0 ? std::normal_distribution<double>(rm[0], rm[2])(generator) : std::uniform_real_distribution<double>(rm[0], rm[1])(generator);

		dfp.vt = vt[2] >= 0 ? std::normal_distribution<double>(vt[0], vt[2])(generator) : std::uniform_real_distribution<double>(vt[0], vt[1])(generator);

		dfp.vr = vr[2] >= 0 ? std::normal_distribution<double>(vr[0], vr[2])(generator) : std::uniform_real_distribution<double>(vr[0], vr[1])(generator);

		dfp.delta = delta[2] >= 0 ? std::normal_distribution<double>(delta[0], delta[2])(generator) : std::uniform_real_distribution<double>(delta[0], delta[1])(generator);

		dfp.s = s[2] >= 0 ? std::normal_distribution<double>(s[0], s[2])(generator) : std::uniform_real_distribution<double>(s[0], s[1])(generator);

		dfp.cd_sm = cd_sm[2] >= 0 ? std::normal_distribution<double>(cd_sm[0], cd_sm[2])(generator) : std::uniform_real_distribution<double>(cd_sm[0], cd_sm[1])(generator);

		dfp.cd_air = cd_air[2] >= 0 ? std::normal_distribution<double>(cd_air[0], cd_air[2])(generator) : std::uniform_real_distribution<double>(cd_air[0], cd_air[1])(generator);

		dfp.cl = cl[2] >= 0 ? std::normal_distribution<double>(cl[0], cl[2])(generator) : std::uniform_real_distribution<double>(cl[0], cl[1])(generator);
	}
};

