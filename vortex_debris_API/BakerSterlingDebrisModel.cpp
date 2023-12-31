#include "pch.h"
#include "BakerSterlingDebrisModel.h"
#include <iostream>
#include <functional>

Vec3 BakerSterlingDebrisModel::computeAcceleration()
{
    const double rm = debrisFlightParams.rm;

    //calculate radius of debris relative to vortex center
    const double x = debrisPos.x - vortexPosX;
    const double y = debrisPos.y;
    double r = hypot(x, y);

    //limit min radius
    r = std::max(r, 0.5 / rm);

    //compute vortex field velocities
    auto [U, V, W] = computeFieldVel(r/rm, (debrisPos.z + debrisParams.z_correction)/zm);

    //compute polar vortex translation velocity
    const double vt_u = x * debrisFlightParams.vt / r;
    const double vt_v = -y * debrisFlightParams.vt / r;

    //compute polar debris velocity relative to vortex center
    //cos(theta) = x/r, sin(theta) = y/r, computing a divison is faster than trig functions
    const double u = (x * debrisVel.x + y * debrisVel.y) / r;
    const double v = (x * debrisVel.y - y * debrisVel.x) / r;

    //compute polar acceleration of debris
    double A = (U + vt_u) - u;
    double B = (V + vt_v) - v;
    double Y = W - debrisVel.z;
    const double R2 = A * A + B * B + Y * Y;
    const double R = sqrt(R2);
    const double SF = k * R2;

    A /= R;
    B /= R;
    Y /= R;

    double dudt = 0.0, dvdt = 0.0, dwdt = 0.0;


    switch (debrisState) {

        case onGround:
            dudt = (debrisFlightParams.cd_sm * A - debrisFlightParams.cl * sqrt(1 - A * A)) * SF + v * v / r;
            dvdt = (debrisFlightParams.cd_sm * B - debrisFlightParams.cl * sqrt(1 - B * B)) * SF;
            dwdt = (debrisFlightParams.cd_sm * Y + debrisFlightParams.cl * sqrt(1 - Y * Y)) * SF - debrisFlightParams.g;

            //dudt = 0.0;
            //dvdt = 0.0;
            dwdt = std::max(dwdt, 0.0);

            //lift off
            if (dwdt > 0.0) {
                debrisFlightParams.loftPos = Vec3(x, y, 0.0);
                //debrisFlightParams.loftSpeed = computeGustSpeed(x, y, 3.0);

            }
            break;

        case transitioning:
            dudt = ((debrisFlightParams.cd_sm * A - debrisFlightParams.cl * sqrt(1 - A * A)) * (1 - debrisPos.z) + (debrisFlightParams.cd_air * A) * debrisPos.z) * SF + v * v / r;
            dvdt = ((debrisFlightParams.cd_sm * B - debrisFlightParams.cl * sqrt(1 - B * B)) * (1 - debrisPos.z) + (debrisFlightParams.cd_air * B) * debrisPos.z) * SF;
            dwdt = ((debrisFlightParams.cd_sm * Y + debrisFlightParams.cl * sqrt(1 - Y * Y)) * (1 - debrisPos.z) + (debrisFlightParams.cd_air * Y) * debrisPos.z) * SF - debrisFlightParams.g;
            break;

        case inAir:
            dudt = (debrisFlightParams.cd_air * A) * SF + v * v / r;
            dvdt = (debrisFlightParams.cd_air * B) * SF;
            dwdt = (debrisFlightParams.cd_air * Y) * SF - debrisFlightParams.g;
            break;
    }

    //compute and return cartesian acceleration
    const double d2xdt2 = (dudt * x - dvdt * y) / r;
    const double d2ydt2 = (dudt * y + dvdt * x) / r;

    return Vec3(d2xdt2, d2ydt2, dwdt);

}

//compute baker sterling field velocity at given r/rm and z/zm
std::array<const double, 3> BakerSterlingDebrisModel::computeFieldVel(const double r, const double z)
{
    const double T = debrisFlightParams.vr;
    const double delta = debrisFlightParams.delta;

    const double rEr = r * Er;
    const double rEr_2_1 = 1 + rEr * rEr;

    const double U = T * ((-(1 + a) * (1 + b) * pow(r, a) * pow(z, b)) / (pow(a, a / (1 + a)) * pow(b, b / (1 + b)) * (1 + pow(r, a + 1)) * (1 + pow(z, b + 1))) + (4 * rEr * z) / (rEr_2_1 * (1 + z * z)));
    const double V = T * ((K * pow(r, c - 1) * pow(log(1 + pow(z, b + 1)), c / (a + 1))) / pow(1 + pow(r, a + 1), c / (a + 1)));
    const double W = T * ((delta * pow(1 + a, 2) * pow(r, a - 1) * log(1 + pow(z, b + 1))) / (pow(a, a / (1 + a)) * pow(b, b / (1 + b)) * pow(1 + pow(r, a + 1), 2)) - (4 * delta * log(1 + z * z)) / (rEr_2_1 * rEr_2_1));

    return { U,V,W };
}


double BakerSterlingDebrisModel::computeFieldMag(double x, double y) {
    const double r = hypot(x, y);
    const double z = (debrisPos.z + debrisParams.z_correction) / zm;

    //compute vortex field velocities
    auto [U, V, W] = computeFieldVel(r / debrisFlightParams.rm, z);

    //compute polar vortex translation velocity
    const double vt_u = x * debrisFlightParams.vt / r;
    const double vt_v = -y * debrisFlightParams.vt / r;

    return sqrt((U + vt_u) * (U + vt_u) + (V + vt_v) * (V + vt_v) + W * W);
}


double BakerSterlingDebrisModel::lazy_Integrate(double(BakerSterlingDebrisModel::* f)(double x, double y), const double x0, const double x1, const double y0, const double y1, const int N) {

    //compute 2d Riemann sum

    double sum = 0.0;
    double sum_in = 0.0;

    const double xs = (x1 - x0) / (double)N;
    const double ys = (y1 - y0) / (double)N;

    for (double y = y0; y < y1; y += ys) {
        for (double x = x0; x < x1; x += xs) {
            sum_in += (this->*f)(x, y) * xs;
        }
        sum += sum_in * ys;
        sum_in = 0.0;
    }
    return sum;
}

double BakerSterlingDebrisModel::simpson_Integrate(double(BakerSterlingDebrisModel::* f)(double x, double y), const double x0, const double x1, const double y0, const double y1, const int N) {

    //compute 2d Simpson's rule integral
    const double xs = (x1 - x0) / (double)N;
    const double ys = (y1 - y0) / (double)N;

    std::vector<std::vector<double>> m;
    m.reserve(N+1);

    //compute simpson sample matrix
    for (int i = 0; i <= N; i++) {
        m.push_back(std::vector<double>());
        m[i].reserve(N + 1);

        for (int j = 0; j <= N; j++) {
            m[i].push_back((this->*f)(x0 + j*xs, y0 + i*ys));
        }
    }

    //sum rows (x) using simpsons rule
    for (int i = 0; i <= N; i++) {
        double sum_in = m[i][0] + m[i][N];

        //odd
        double sum_odd = 0.0;
        for (int j = 1; j < N; j += 2) {
            sum_odd += m[i][j];
        }

        //even
        double sum_even = 0.0;
        for (int j = 2; j < N; j += 2) {
            sum_even += m[i][j];
        }

        sum_in += 4.0 * sum_odd + 2.0 * sum_even;

        sum_in *= (xs / 3.0);

        m[i][0] = sum_in;
    }

    //sum summed column using simpsons rule
    double sum = m[0][0] + m[N][0];

    //odd
    double sum_odd = 0.0;
    for (int i = 1; i < N; i += 2) {
        sum_odd += m[i][0];
    }

    //even
    double sum_even = 0.0;
    for (int i = 2; i < N; i += 2) {
        sum_even += m[i][0];
    }

    sum += 4.0 * sum_odd + 2.0 * sum_even;

    sum *= (ys / 3.0);

    return sum;
}


double BakerSterlingDebrisModel::computeGustSpeed(const double x, const double y, const double gustTime) {

    const double sf = (gustTime / 2.0) * debrisFlightParams.vt;
    const double x1 = x + sf;
    const double x0 = x - sf;
    const double y1 = y + sf;
    const double y0 = y - sf;

    double I = simpson_Integrate(&BakerSterlingDebrisModel::computeFieldMag, x0, x1, y0, y1, 100);

    const double A = gustTime * gustTime * debrisFlightParams.vt * debrisFlightParams.vt;

    return I / A;
}

