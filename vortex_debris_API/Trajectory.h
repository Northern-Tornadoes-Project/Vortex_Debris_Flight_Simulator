#pragma once
#include "Vec3.h"
#include <iostream>
#include <vector>

class Trajectory
{
public:
	std::vector<Vec3> traj;

	Trajectory() {};

	void add(Vec3& v) {
		traj.push_back(Vec3(v.x, v.y, v.z));
	}

	double** exportTraj() {
		std::vector<double*>* exTraj = new std::vector<double*>();

		for (int i = 0; i < traj.size(); i++) {
			exTraj->push_back(new double[] {traj[i].x, traj[i].y, traj[i].z});
		}

		return (double**)&exTraj->at(0);
	}

};

