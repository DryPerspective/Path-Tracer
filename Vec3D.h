#ifndef VEC3D_INHERITED_CLASS
#define VEC3D_INHERITED_CLASS
#pragma once

/*
* A basic inherited class to add some functionality specific to this project to our vectors, with the added bonus of auotmatically setting the template parameter once here for every other use.
* Inheritance seemed the correct approach here - adding functionality which is incredibly specific to this path tracer project to the general vector object in the library only adds bloat in future projects,
* and it seems unnecessary to go the composition route and have to re-expose the entire PhysicsVector interface for the sake of four new functions.
*/

#include<random>
#include<ctime>

#include "PhysicsVector.h"


class Vec3D : public Physics::PhysicsVector<3>
{
public:
	//Constructors. As we have no new member data to set up, we can rely on the base class' constructors to do most of the heavy lifting.
	Vec3D() : PhysicsVector<3>() {}
	Vec3D(const PhysicsVector<3>& inVector) : PhysicsVector<3>(inVector) {}
	Vec3D(const std::initializer_list<double>& inList) : PhysicsVector<3>(inList) {}

	//NB: The base class has a virtual default destructor so we should be safe for destruction either way.

	

	//First, a "scale-by-vector" function. In pure math terms this is pretty meaningless but it transforms {a,b,c} and {x,y,z} to {ax,by,cz}
	Vec3D scaledByVector(const Vec3D inVector) const {
		Vec3D output;
		for (int i = 0; i < 3; ++i) {
			output.setAt(i, (this->at(i) * inVector.at(i)));
		}
		return output;
	}

	//Scattering functions
	//Generate a vector with randomly assigned values between two points.
	static Vec3D randVector(double inMin, double inMax) {
		std::uniform_real_distribution<double> distribution{ inMin, inMax };
		static std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
		return Vec3D{ distribution(mersenne), distribution(mersenne), distribution(mersenne) };
	}

	//Generate a random vector inside the unit sphere.
	static Vec3D randInUnitSphere() {
		while (true) {
			Vec3D Vec{ randVector(-1,1) };	//Generate a random vector inside the unit cube
			if (Vec.lengthSquared() >= 1)continue;	//Reject it if it's outside the unit sphere.
			return Vec;								//Otherwise return it.
		}
	}
	//A closer approximation to true Lambertian scattering for diffuse materials.
	//Taking the unit vector biases samples more towards the surface normal (must be on the surface of the unit sphere rather than just inside it)
	//Which, as it turns out, is closer to what Lambertian scattering does in reality.
	static Vec3D randLambertianUnitSphere() {
		return randInUnitSphere().getUnitVector();
	}
	//Generate a random vector inside the unit disk. Primarily used to simulate a camera aperture
	static Vec3D randInUnitDisk() {
		while (true) {
			Vec3D Vec{ randVector(-1,1) };					//Start off with a vector on the unit cube
			Vec.setZ(0);									//Then set its Z component to 0 to get one on the disk.
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
	static Vec3D smoothReflect(const Vec3D& inRay, const  Vec3D& inNormal) {
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
	static Vec3D refract(Vec3D inR, Vec3D inNormal, const double etaOverEtaPrime) {
		Vec3D unitVectorR{ inR.getUnitVector() };
		auto cosTheta{ fmin(-unitVectorR.innerProduct(inNormal),1.0) };			//Cos theta can never be above 1, but in computing we may get something slightly above due to floating points.
		Vec3D rPrimePerp{ (unitVectorR + inNormal.scaledBy(cosTheta)).scaledBy(etaOverEtaPrime) };
		Vec3D rPrimeParallel{ inNormal.scaledBy(-sqrt(fabs(1.0 - rPrimePerp.lengthSquared()))) };
		return rPrimeParallel + rPrimePerp;
	}




};

#endif