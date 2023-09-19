#include "pch.h"
#include "BakerSterlingDebrisModel.h"
#include <iostream>

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

    //compute polar debris velocity relative to vortex center
    //cos(theta) = x/r, sin(theta) = y/r, computing a divison is faster than trig functions
    const double u = (x * debrisVel.x + y * debrisVel.y) / r;
    const double v = (x * debrisVel.y - y * debrisVel.x) / r;

    //compute polar acceleration of debris
    double A = U - u;
    double B = V - v;
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
                debrisFlightParams.loftSpeed = sqrt(U * U + V * V + W * W);
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
    const double W = T * ((delta * pow(1 + a, 2) * pow(r, a - 1) * log(1 + pow(z, b + 1))) / (pow(a, a / (1 + a)) * pow(b, b / (1 + b)) * pow(1 + pow(r, a + 1), 2)) - (4 * delta * log(1 + z*z)) / (rEr_2_1 * rEr_2_1));

    return { U,V,W };
}

