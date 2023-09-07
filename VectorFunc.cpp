#include "VectorFunc.h"

#include <random>
#include <mutex>


namespace dp {

	/*
	* While a global in the cpp file would ordinarily be all sorts of bad practice, this class specifically needs to be able to generate random numbers on the fly.
	* It was decided that a single static prng to be called by multiple functions was likely more performant than each function call needing to materialise a PRNG, call it once
	* and destroy it again.
	* We use a mutex to prevent data races
	*/
	namespace {
		std::mt19937 mersenne{ std::random_device{}() };
		std::uniform_real_distribution<double> dist{ 0.0, 1.0 };
		std::mutex m;
	}

	//A tad on the mathematically meaningless side, but we simply iterate to transform (a,b,c) * (x,y,z) to (ax,by,cz)
	dp::PhysicsVector<3> scaledByVector(const dp::PhysicsVector<3>& toScale, const dp::PhysicsVector<3>& scaleBy) {
		dp::PhysicsVector<3> output;
		for (int i = 0; i < 3; ++i) {
			output.setAt(i, (toScale[i] * scaleBy[i]));
		}
		return output;
	}


	dp::PhysicsVector<3> randVector(double inMin, double inMax) {
		std::uniform_real_distribution<double> distribution{ inMin, inMax };
		std::lock_guard<std::mutex> lck(m);
		return dp::PhysicsVector<3>{ distribution(mersenne), distribution(mersenne), distribution(mersenne) };
	}

	dp::PhysicsVector<3> randInUnitSphere() {

		std::lock_guard<std::mutex> lck(m);
		double theta{ 2 * 3.14159265358979323846 * dist(mersenne) };	//Magic pi. Not an ideal piece of code but we only use it once.
		double phi = std::acos(1 - 2 * dist(mersenne));
		return dp::PhysicsVector<3>{ std::sin(phi)* std::cos(theta), std::sin(phi)* std::sin(theta), std::cos(phi) };

		/*
		while (true) {
			Vec3D Vec{ randVector(-1,1) };			//Generate a random vector inside the unit cube
			if (Vec.lengthSquared() >= 1)continue;	//Reject it if it's outside the unit sphere.
			return Vec;								//Otherwise return it.
		}
		*/
	}

	dp::PhysicsVector<3> randLambertianUnitSphere() {
		return randInUnitSphere().getUnitVector();
	}

	dp::PhysicsVector<3> randInUnitDisk() {
		while (true) {
			dp::PhysicsVector<3> Vec{ randVector(-1,1) };	//Start off with a vector on the unit cube
			Vec.z() = 0;										//Then set its Z component to 0 to get one on the disk.
			if (Vec.lengthSquared() >= 1)continue;				//Reject it if it's outside the unit disk
			return Vec;											//Otherwise return it.
		}
	}

	dp::PhysicsVector<3> smoothReflect(const dp::PhysicsVector<3>& inRay, const  dp::PhysicsVector<3>& inNormal) {
		return inRay - inNormal.scaledBy(2 * inRay.innerProduct(inNormal));
	}

	dp::PhysicsVector<3> refract(const dp::PhysicsVector<3>& inR, const dp::PhysicsVector<3>& inNormal, const double etaOverEtaPrime) {
		dp::PhysicsVector<3> unitVectorR{ inR.getUnitVector() };
		auto cosTheta{ fmin(-unitVectorR.innerProduct(inNormal),1.0) };			//Cos theta can never be above 1, but in computing we may get something slightly above due to floating points.
		dp::PhysicsVector<3> rPrimePerp{ (unitVectorR + inNormal.scaledBy(cosTheta)).scaledBy(etaOverEtaPrime) };
		dp::PhysicsVector<3> rPrimeParallel{ inNormal.scaledBy(-sqrt(fabs(1.0 - rPrimePerp.lengthSquared()))) };
		return rPrimeParallel + rPrimePerp;
	}

}