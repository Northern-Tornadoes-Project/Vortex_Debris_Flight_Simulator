#include "pch.h"
#include "API_Interface.h"
#define NOMINMAX
using namespace std;


//displays a progressbar for a for loop
//i = current index of loop
//total = total iterations of loop
void updateProgressBar(int i, int total) {
    //assumes index starts at 1
    ++i;
    int barLength = 50;
    float progress = (float)i / (float)total;
    int pos = (int)(progress * (float)barLength);

    std::cout << "|" << std::string(pos, (char)(219)) << std::string(barLength - pos, ' ') << "| " << i << "/" << total << "\r";
}

double** matchTrajMonteCarlo(double params[]){

	const double NUM_OF_VALID_TRAJECTORIES = params[0];

	//list of valid sets of parameters
	vector<DebrisFlightParams> validParams;
	validParams.reserve(NUM_OF_VALID_TRAJECTORIES);

	RandomParameterGenerator randParamGen = RandomParameterGenerator();
	unpackRandParams(params, randParamGen);

	int totalSims = 0;

	//set up parallel code execution to use all logical CPU processors
	int p = omp_get_num_procs();
	omp_set_num_threads(p);

	updateProgressBar(0, NUM_OF_VALID_TRAJECTORIES);

	//tell compiler this section should be run in parallel
	#pragma omp parallel
	{
		while (validParams.size() <= NUM_OF_VALID_TRAJECTORIES) {

			DebrisFlightParams dfp = DebrisFlightParams();
			dfp.width = params[1];
			dfp.traj_min = params[2];
			dfp.traj_max = params[3];

			//only one parallel thread can enter a critcal section at a time
			//this is to prevent synchronization problems where two threads
			//overwrite the same resource at the same time.
			#pragma omp critical
			{
				totalSims++;
				randParamGen.generate(dfp);
			}

			DebrisParams dp = DebrisParams();
			dp.l = params[4];
			dp.rho_m = params[5];
			dp.z_correction = dfp.rm * dfp.delta;

			//create simulation model using parameters
			BakerSterlingDebrisModel model = BakerSterlingDebrisModel(dp, dfp);

			//run simulation
			DebrisFlightParams result = model.runSimulation();

			//add valid trajectories to list
			if (result.valid) {

				#pragma omp critical
				{
					validParams.push_back(result);
					updateProgressBar(validParams.size(), NUM_OF_VALID_TRAJECTORIES);
				}
			}
		}
	}

	cout << "\nTotal Simulations: " << totalSims << " -> " << ((double)NUM_OF_VALID_TRAJECTORIES/(double)totalSims)*100.0 << "% success rate\n\n";

	vector<double*>* packedValidParams = new vector<double*>();
	packedValidParams->reserve(NUM_OF_VALID_TRAJECTORIES);

	for (auto& p : validParams) {
		packedValidParams->push_back(packDebrisFlightParams(p));
	}

	return (double**)&packedValidParams->at(0);
}

double** getTraj(double params[]){
	DebrisFlightParams dfp = DebrisFlightParams();
	dfp.width = params[0];

	dfp.rm = params[3];
	dfp.vt = params[4];
	dfp.vr = params[5];
	dfp.delta = params[6];
	dfp.s = params[7];
	dfp.cd_sm = params[8];
	dfp.cd_air = params[9];
	dfp.cl = params[10];

	DebrisParams dp = DebrisParams();
	dp.l = params[1];
	dp.rho_m = params[2];
	dp.z_correction = dfp.rm * dfp.delta;

	BakerSterlingDebrisModel model = BakerSterlingDebrisModel(dp, dfp);

	Trajectory traj = model.simulateTrajectory();
	auto v = Vec3(nan("0"), nan("0"), nan("0"));
	traj.add(v);

	return traj.exportTraj();
}

void unpackRandParams(double params[], RandomParameterGenerator& randParamGen) {
	randParamGen.rm = { params[6], params[7], params[8] }; // {min or mean, max, sd} if sd < 0, uses uniform distribution instead
	randParamGen.vt = { params[9], params[10], params[11] };
	randParamGen.vr = { params[12], params[13], params[14] };
	randParamGen.delta = { params[15], params[16], params[17] };
	randParamGen.s = { params[18], params[19], params[20] };
	randParamGen.cd_sm = { params[21], params[22], params[23] };
	randParamGen.cd_air = { params[24], params[25], params[26] };
	randParamGen.cl = { params[27], params[28], params[29] };
}

double* packDebrisFlightParams(DebrisFlightParams& dfp) {
	return new double[9] { dfp.loftSpeed, dfp.rm, dfp.vt, dfp.vr, dfp.delta, dfp.s, dfp.cd_sm, dfp.cd_air, dfp.cl};
}
