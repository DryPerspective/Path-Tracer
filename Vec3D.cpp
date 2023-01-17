#include "Vec3D.h"

//A tad on the mathematically meaningless side, but we simply iterate to transform (a,b,c) * (x,y,z) to (ax,by,cz)
Vec3D Vec3D::scaledByVector(const Vec3D& inVector) const {
	Vec3D output;
	for (int i = 0; i < 3; ++i) {
		output.setAt(i, (this->at(i) * inVector.at(i)));
	}
	return output;
}


Vec3D Vec3D::randVector(double inMin, double inMax) {
	std::uniform_real_distribution<double> distribution{ inMin, inMax };
	static std::mt19937 mersenne{ std::random_device{}() };
	return Vec3D{ distribution(mersenne), distribution(mersenne), distribution(mersenne) };
}

Vec3D Vec3D::randInUnitSphere() {
	while (true) {
		Vec3D Vec{ randVector(-1,1) };			//Generate a random vector inside the unit cube
		if (Vec.lengthSquared() >= 1)continue;	//Reject it if it's outside the unit sphere.
		return Vec;								//Otherwise return it.
	}
}

Vec3D Vec3D::randLambertianUnitSphere() {
	return randInUnitSphere().getUnitVector();
}

Vec3D Vec3D::randInUnitDisk() {
	while (true) {
		Vec3D Vec{ randVector(-1,1) };					//Start off with a vector on the unit cube
		Vec.setZ(0);									//Then set its Z component to 0 to get one on the disk.
		if (Vec.lengthSquared() >= 1)continue;			//Reject it if it's outside the unit disk
		return Vec;										//Otherwise return it.
	}
}

Vec3D Vec3D::smoothReflect(const Vec3D& inRay, const  Vec3D& inNormal) {
	return inRay - inNormal.scaledBy(2 * inRay.innerProduct(inNormal));
}

Vec3D Vec3D::refract(const Vec3D& inR, const Vec3D& inNormal, const double etaOverEtaPrime) {
	Vec3D unitVectorR{ inR.getUnitVector() };
	auto cosTheta{ fmin(-unitVectorR.innerProduct(inNormal),1.0) };			//Cos theta can never be above 1, but in computing we may get something slightly above due to floating points.
	Vec3D rPrimePerp{ (unitVectorR + inNormal.scaledBy(cosTheta)).scaledBy(etaOverEtaPrime) };
	Vec3D rPrimeParallel{ inNormal.scaledBy(-sqrt(fabs(1.0 - rPrimePerp.lengthSquared()))) };
	return rPrimeParallel + rPrimePerp;
}