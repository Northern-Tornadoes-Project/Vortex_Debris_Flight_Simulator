#include <iostream>
#include <string>
#include <omp.h>
#include <matplot/matplot.h>
#include <matplot/axes_objects/histogram.h>

#include <API_Interface.h>


#define NUM_OF_VALID_TRAJECTORIES 10000

using namespace std;

struct doubleArrayComparator {
	bool operator()(double* left, double* right) const
	{
		return left[0] < right[0];
	}
};


void exportParamsToCSV(string filename, vector<double*>& data, double* p) {

	ofstream csvFile(filename + ".csv");

	csvFile << "Valid Trajectories, width, trajectory min, trajectory max\n";

	csvFile << std::format("{},{},{},{}\n", p[0], p[1], p[2], p[3]);

	csvFile << "Loft Speed, l, rho_m, rm, vt, vr, delta, s, cd_sm, cd_air, cl\n";

	for (auto& d : data) {
		csvFile << std::format("{},{},{},{},{},{},{},{},{},{},{}\n", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10]);
	}

	csvFile.close();
}


void exportTrajToCSV(string filename, array<vector<double>, 3>& split_traj) {
	ofstream csvFile(filename + ".csv");

	csvFile << "x, y, z, t (assuming dt = 0.001)\n";
	double const dt = 0.001;

	for (int i = 0; i < split_traj[0].size(); i++) {
		csvFile << std::format("{},{},{},{}\n", split_traj[0][i], split_traj[1][i], split_traj[2][i], i*dt);
	}

	csvFile.close();
}


int main() { 

	double p[34] = 
	{ 
		NUM_OF_VALID_TRAJECTORIES,		// NUM_OF_VALID_TRAJECTORIES
		1600.0,							// width
		0.0,							// traj min set <= 0 for loft threshold speed
		100.0,							// traj max
										// Randomly generated parameters {min or mean, max, sd} if sd < 0, uses uniform distribution instead

		1.22, 1.22, -1.0,				// l, Characteristic Length of Debris
		145.0, 190.0, -1.0,				// rho_m Density of Debris						
		160.0, 320.0, -1.0,				// rm, Core Radiusm
		15.0, 19.0, -1.0,				// vt, Translational Speed
		30.0, 140.0, -1.0,				// vr, Max Radial Speed
		0.1, 0.2, -1.0,					// delta, Terrain Roughness
		0.7, 1.3, -1.0,					// s, Swirl Ratio
		0.71, 0.83, -1.0,				// cd_sm, Drag Coeffcient
		1.2, 1.5, -1.0,					// cd_air, Drag Coeffcient
		0.34, 0.46, -1.0				// cl, Drag Coeffcient
	};
	auto r = matchTrajMonteCarlo(p);

	//compute wind velocities of results
	vector<double*> results;
	results.reserve(NUM_OF_VALID_TRAJECTORIES);

	for (int i = 0; i < NUM_OF_VALID_TRAJECTORIES; i++) {
		results.push_back(r[i]);
	}

	sort(results.begin(), results.end(), doubleArrayComparator());

	cout << "Min Vel: " << results[0][0] << "m/s\n\n";
	cout << "Max Vel: " << results[NUM_OF_VALID_TRAJECTORIES - 1][0] << "m/s\n\n";
	cout << "Median Vel: " << results[(int)(NUM_OF_VALID_TRAJECTORIES / 2.0)][0] << "m/s\n\n";

	//export to csv
	//exportParamsToCSV("haybail_parameters", results, p);


	////plot cdf
	//array<vector<double>, 2> cdfDist;

	//for (int i = 0; i < NUM_OF_VALID_TRAJECTORIES; i++) {
	//	cdfDist[0].push_back(results[i][0]);
	//	cdfDist[1].push_back((double)(i+1) / (double)NUM_OF_VALID_TRAJECTORIES);
	//}

	////smooth cdf using mean filter
	///*int filter_size = 9;
	//for (int i = 0; i < NUM_OF_VALID_TRAJECTORIES; i++) {
	//	double sum = 0.0;
	//	double count = 0.0;

	//	for (int j = max(0, i - filter_size/2); j < min(NUM_OF_VALID_TRAJECTORIES, i + filter_size / 2 + 1); j++) {
	//		sum += cdfDist[0][j];
	//		count += 1.0;
	//	}

	//	cdfDist[0][i] = sum / count;
	//}*/

	//auto cdfPlot = matplot::plot(cdfDist[0], cdfDist[1]);
	//matplot::yrange({ -0.05, 1.05 });
	//matplot::xlabel("Loft Speed (m/s)");
	//matplot::ylabel("Percentage %");
	//matplot::title("CDF Plot");
	//matplot::show();

	//plot median trajectory (needs to be updated)
	double* medianResult = results[(int)(NUM_OF_VALID_TRAJECTORIES / 2.0)];
	double medianP[11] = { 640.0, medianResult[1], medianResult[2], medianResult[3], medianResult[4], medianResult[5], medianResult[6], medianResult[7], medianResult[8], medianResult[9], medianResult[10] };

	auto traj = getTraj(medianP);
	auto trajRK4 = getTrajRK4(medianP);

	array<vector<double>, 3> split_traj;
	array<vector<double>, 3> split_traj_RK4;

	int i = 0;
	while (!isnan(traj[i][0])) {
		split_traj[0].push_back(traj[i][0]);
		split_traj[1].push_back(traj[i][1]);
		split_traj[2].push_back(traj[i][2]);
		i++;
	}
	//auto l = matplot::plot3(split_traj[0], split_traj[1], split_traj[2]);

	i = 0;
	while (!isnan(trajRK4[i][0])) {
		split_traj_RK4[0].push_back(trajRK4[i][0]);
		split_traj_RK4[1].push_back(trajRK4[i][1]);
		split_traj_RK4[2].push_back(trajRK4[i][2]);
		i++;
	}
	auto l = matplot::plot3(split_traj[0], split_traj[1], split_traj[2], split_traj_RK4[0], split_traj_RK4[1], split_traj_RK4[2]);

	//exportTrajToCSV("haybail_trajectory", split_traj);

	matplot::xlabel("x");
	matplot::ylabel("y");
	matplot::zlabel("z");
	matplot::show();

	//plot histogram of loft speeds
	/*vector<double> loftSpeeds;

	for(auto& p : results){
		loftSpeeds.push_back(p[0]);
	}

	auto h = matplot::hist(loftSpeeds, matplot::histogram::binning_algorithm::automatic, matplot::histogram::normalization::probability);
	
	matplot::title("Monte Carlo Debris Flight Simulations");
	matplot::xlabel("loft speed m/s");
	matplot::ylabel("percentage %");
	matplot::show();*/
	
}

//IGNORE

/*//list of valid sets of parameters
	vector<DebrisFlightParams> validParams;
	validParams.reserve(NUM_OF_VALID_TRAJECTORIES);

	//Enter parameter ranges here
	RandomParameterGenerator randParamGen = RandomParameterGenerator();
	randParamGen.rm = { 64.0, 128.0, -1.0 }; // {min or mean, max, sd} if sd < 0, uses uniform distribution instead
	randParamGen.vt = { 6.0, 10.0, -1.0 };
	randParamGen.vr = { 90.0, 0.0, 10.0 };
	randParamGen.delta = { 0.1, 0.2, -1.0 };
	randParamGen.s = { 1.0, 0.0, 0.1 };
	randParamGen.cd_sm = { 2.0, 0.0, 0.05 };
	randParamGen.cd_air = { 1.5, 0.0, 0.05 };
	randParamGen.cl = { 0.3, 0.0, 0.025 };

	//set up parallel code execution to use all logical CPU processors
	int p = omp_get_num_procs();
	omp_set_num_threads(p);

	//tell compiler this section should be run in parallel
	#pragma omp parallel
	{
		while (validParams.size() <= NUM_OF_VALID_TRAJECTORIES) {

			//Change flight parameters here
			DebrisFlightParams dfp = DebrisFlightParams();
			dfp.width = 640.0;
			dfp.traj_min = 80.0;
			dfp.traj_max = 100.0;

			//only one parallel thread can enter a critcal section at a time
			//this is to prevent synchronization problems where two threads
			//overwrite the same resource at the same time.
			#pragma omp critical
			{
				randParamGen.generate(dfp);
			}

			//Change debris parameters here
			DebrisParams dp = DebrisParams();
			dp.l = 3.35;
			dp.rho_m = 72.0;
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
				}
			}
		}
	}

	//compute wind velocities of results
	vector<double> windVels;
	windVels.reserve(NUM_OF_VALID_TRAJECTORIES);

	for (auto& p : validParams) {
		windVels.push_back(p.loftSpeed);
	}

	sort(windVels.begin(), windVels.end());

	cout << "Min Vel: " << windVels[0] << "m/s\n\n";
	cout << "Max Vel: " << windVels[NUM_OF_VALID_TRAJECTORIES - 1] << "m/s\n\n";
	cout << "Median Vel: " << windVels[NUM_OF_VALID_TRAJECTORIES/2.0] << "m/s\n\n"; */

/*while (validParams.size() < NUM_OF_VALID_TRAJECTORIES) {

		DebrisFlightParams dfp = DebrisFlightParams();
		dfp.width = 640.0;
		dfp.traj_min = 80.0;
		dfp.traj_max = 100.0;

		randParamGen.generate(dfp);

		DebrisParams dp = DebrisParams();
		dp.l = 3.35;
		dp.rho_m = 72.0;
		dp.z_correction = dfp.rm * dfp.delta;


		BakerSterlingDebrisModel model = BakerSterlingDebrisModel(dp, dfp);

		auto result = model.runSimulation();

		if (result.valid) {
			validParams.push_back(result);
		}
	}*/