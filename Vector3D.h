#ifndef VECTOR3D_H
#define VECTOR3D_H
#pragma once
/* A 3D vector template class which contains all the functions needed for the necessary vector calculus used in this project.
*  Most of the functions should either be self explanatory or commented enough to follow.
* I will just note that I'm aware that there are times where I use the this keyword when not strictly necessary.
* This is usually done in functions where there are multiple vectors under consideration and using the keyword absolutely kills all possible ambiguity about which Vector we're dealing with.
* I'm aware that from a C++ purist point of view that's not necessary as it's usually quite apparent, but better safe.
*/



#include <cmath>	//For sqrt, fmin, and fabs.
#include <limits>	//For epsilon function when calculating the unit vector
#include <random>	//To allow random vector generation.
#include <ctime>	//To get system time for a seed for the RNG.

template<typename T>
class Vector3D
{
private:

	T m_X, m_Y, m_Z;


public:
	//Constructors
	Vector3D():m_X(T(0)), m_Y(T(0)), m_Z(T(0)) {};
	Vector3D(T inX, T inY, T inZ) :m_X(inX), m_Y(inY), m_Z(inZ) {};

	//Getters
	T getX() const { return m_X; }
	T getY() const { return m_Y; }
	T getZ() const { return m_Z; }

	//Operator overloads
	Vector3D operator-() const { return Vector3D(-m_X, -m_Y, -m_Z); }
	Vector3D operator+(const Vector3D& inVector) const { return Vector3D((this->getX() + inVector.getX()), (this->getY() + inVector.getY()), (this->getZ() + inVector.getZ())); }
	Vector3D operator-(const Vector3D& inVector) const { return Vector3D((this->getX() - inVector.getX()), (this->getY() - inVector.getY()), (this->getZ() - inVector.getZ())); }
	Vector3D operator/(T inScalar) const { return this->scaledBy(static_cast<T>(1) / inScalar); }
	Vector3D operator+=(const Vector3D& inVector) {
		m_X += inVector.getX();
		m_Y += inVector.getY();
		m_Z += inVector.getZ();
		return *this;
	}

	//Vector Utility functions
	Vector3D scaledBy(T inScalar) const { return Vector3D((this->getX() * inScalar), (this->getY() * inScalar), (this->getZ() * inScalar)); }
	T innerProduct(Vector3D inVector) const { return (m_X * inVector.m_X + m_Y * inVector.m_Y + m_Z * inVector.m_Z); }
	T lengthSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z; }
	T length() const { return sqrt(this->lengthSquared()); }
	Vector3D getUnitVector() const {
		if (this->length() <= std::numeric_limits<T>::epsilon()) return Vector3D(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); //If you somehow have a vector of length 0, this prevents divide-by-zero errors
		Vector3D unitVector(m_X, m_Y, m_Z);
		return unitVector.scaledBy(static_cast<T>(1) / (this->length()));
	}
	static Vector3D vectorProduct(const Vector3D& inVec1, const Vector3D& inVec2) {
		return Vector3D(inVec1.m_Y * inVec2.m_Z - inVec1.m_Z * inVec2.m_Y,
						inVec1.m_Z * inVec2.m_X - inVec1.m_X * inVec2.m_Z,
						inVec1.m_X * inVec2.m_Y - inVec1.m_Y * inVec2.m_X);
	}
	//Generate a vector with randomly assigned values between two points.
	static Vector3D randVector(T inMin, T inMax) {
		std::uniform_real_distribution<T> distribution{ inMin, inMax };
		static std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
		return Vector3D(distribution(mersenne), distribution(mersenne), distribution(mersenne));
	}
	//A function to detemine if a vector has near-zero length. Used to catch fringe issues which may occur when scattering.
	bool isNearZero() const {
		if (this->length() <= std::numeric_limits<T>::epsilon())return true;
		return false;
	}
	Vector3D<T> scaledByVector(Vector3D<T> inVector) {
		return Vector3D<T>(m_X * inVector.getX(), m_Y * inVector.getY(), m_Z * inVector.getZ());
	}


	//Scattering functions
	//Generate a random vector inside the unit sphere.
	static Vector3D randInUnitSphere() {
		while (true) {
			Vector3D<T> Vec{ randVector(-1,1) };	//Generate a random vector inside the unit cube
			if (Vec.lengthSquared() >= 1)continue;	//Reject it if it's outside the unit sphere.
			return Vec;								//Otherwise return it.
		}
	}
	//A closer approximation to true Lambertian scattering for diffuse materials.
	//Taking the unit vector biases samples more towards the surface normal (must be on the surface of the unit sphere rather than just inside it)
	//Which, as it turns out, is closer to what Lambertian scattering does in reality.
	static Vector3D randLambertianUnitSphere() {
		return randInUnitSphere().getUnitVector();
	}
	//Generate a random vector inside the unit disk. Primarily used to simulate a camera aperture
	static Vector3D randInUnitDisk() {
		while (true) {
			Vector3D<T> Vec{ randVector(-1,1) };			//Start off with a vector on the unit cube
			Vec.m_Z = 0;									//Then set its Z component to 0 to get one on the disk.
			if (Vec.lengthSquared() >= 1)continue;			//Reject it if it's outside the unit disk
			return Vec;										//Otherwise return it.
		}
	}
	//A function to simulate perfect reflection on a smooth surface. We assume angle of incidence equals angle of reflection, and do a little vector mathmatics.
	//First draw your incoming vector V as though it continued straight through the material. Let B be the displacement from the material inner surface to the tip of your vector.
	//It's simple mathematics that the tip of your reflected vector will exist at the tip of your vector inside the material + 2B.
	//The length of B will be given by V*N (dot product), where N is the surface normal. 
	//Therefore the tip of the reflected vector will exist (V*N) distance above the surface, which is in the direction of N.
	//Therefore our reflected vector can be calculated by V - (V*N)N. The minus sign comes from the fact that V and N are in opposite directions.
	static Vector3D<T> smoothReflect(Vector3D<T> inRay, Vector3D<T> inNormal) {
		return inRay - inNormal.scaledBy(2 * inRay.innerProduct(inNormal));
	}
	//How we simulate refraction. Snell's Law: n1 sin(theta1) = n2 sin(theta2), where n (eta) corresponds to the refractive index and the theta is the angle between the ray and the normal.
	//So for air into glass, we get n_air sin(angle of incidence) = n_glass sin(angle of transmission).
	//Since we know both refractive indices and can calculate the angle of incidence for a given ray, we want to solve for sin(theta2) to calculate the trajectory of the transmitted ray.
	//For teminology's sake, unprimed values(n, theta, R) are incident, and primed (n', theta', R') are transmitted. R is the incident/transmitted vector, and N is the vector normal.
	// We want to solve for R', the vector representing the direction of transmission. This is best done by separating R' into components perpendicular and parallel to the normal.
	// After some vector calculus, we can find that R'_perp = n/n' * (R - cos(theta)N)) and that R'_parallel = -sqrt(1-|R'_perp|^2 N).
	//Substituting the identity A*B = |A||B|cost(theta) and confining our A and B to unit vectors(giving A*B = cos(theta)), we can express R'_perp in entirely known quantities
	// R'_perp = n/n' (R + (-R*N)N).
	static Vector3D<T> refract(const Vector3D<T>& inR, const Vector3D<T>& inNormal, const T etaOverEtaPrime) {
		Vector3D<T> unitVectorR{ inR.getUnitVector() };
		auto cosTheta{ fmin(-unitVectorR.innerProduct(inNormal),1.0) };			//Cos theta can never be above 1, but in computing we may get something slightly above due to floating points.
		Vector3D<T> rPrimePerp{ (unitVectorR + inNormal.scaledBy(cosTheta)).scaledBy(etaOverEtaPrime) };
		Vector3D<T> rPrimeParallel{ inNormal.scaledBy(-sqrt(fabs(1.0 - rPrimePerp.lengthSquared()))) };
		return rPrimeParallel + rPrimePerp;
	}	

};

#endif

