#include <cmath>
#include <stdexcept>
#include <iostream>

float accel_time(float vi, float vt, float cd, float m, float f)
{
	// Drag force is normally calulated as F_D = 0.5 * rho * v^2 * C_D * A
	// for simplicity, cd here refers to the combined drag coefficient, 
	// in which the drag force is equal to cd * v^2. cd = 0.5 * rho * C_D * A

	if (cd <= 0) {
		throw std::invalid_argument("Drag coefficient must be positive");
	}
	if (m <= 0) {
		throw std::invalid_argument("Mass must be positive");
	}
	
	if (f == 0) {
		float A = m / (cd * vt);
		float B = m / (cd * vi);
		return A - B;
	}
	
	// set f to be positive
	float direction = f / abs(f);
	f *= direction;
	vi *= direction;
	vt *= direction;

	if (vi == 0) {

		float A = m / (2 * std::sqrt(cd * f));
		double B = abs(cd * vt - std::sqrt(cd * f));
		double C = abs(cd * vt + std::sqrt(cd * f));
		
		return - A * std::log(B / C);
	}
	if (vi < 0 && vt < 0) {

		float A = m / (2 * std::sqrt(cd * f));
		float B = cd * vi / std::sqrt(cd * f);
		float C = cd * vt / std::sqrt(cd * f);

		return A * std::atan(B) - A * std::atan(C);
	}
	if (vi > 0 && vt > 0) {
		float t1 = accel_time(0, vi, cd, m, f);
		float t2 = accel_time(0, vt, cd, m, f);

		return t2 - t1;
	}
	if (vi < 0 && vt > 0) {
		float t1 = accel_time(vi, 0, cd, m, f);
		float t2 = accel_time(0, vt, cd, m, f);
		return t2 + t1;
	}
}